/* $Id$
 * LCSensitiveDetector.cc
 * 2ModLumiCal
 *
 *  Created on: Mar 26, 2009
 *      Author: aguilar
 */

// time optimization
#include <time.h>

#include "LCSensitiveDetector.hh"
#include "LCHit.hh"
#include "LCEventAction.hh"
#include "Setup.hh"



LCSensitiveDetector::LCSensitiveDetector(G4String sdname,
                                         G4double rhomin,
                                         G4double PhiOffset,
                                         G4double DimRho,
                                         G4double DimPhi,
                                         G4int    nCellRho,
                                         G4int    nCellPhi,
                                         G4bool   cellvirtual)
    : G4VSensitiveDetector(sdname),
      HCID(-1),
      SDName(sdname), // this sets string SDName to sdname
      hitsColl(0),
      CalRhoMin( rhomin ),
      CalPhiOffset( PhiOffset ),
      cellDimRho( DimRho),
      cellDimPhi( DimPhi ),
      NumCellsRho( nCellRho ), 
      NumSectorsPhi( nCellPhi ),
      VirtualCell( cellvirtual )
{
    collName = SDName+"_HC"; // not dynamic - name becomes LumiCalSD_HC
    collectionName.insert(collName); // a G4VSensitiveDetector element
    origin = G4ThreeVector();
    hitMap = new LCHitMap;

#ifdef LCSD_DEBUG
    G4cout << "SD created <" << SDName << ">" << G4endl;
#endif
}

LCSensitiveDetector::~LCSensitiveDetector()
{
    hitMap->clear();
    delete hitMap;
#ifdef LCSD_DEBUG
    G4cout << "SD destructed <" << SDName << ">" << G4endl;
#endif
}

void LCSensitiveDetector::Initialize(G4HCofThisEvent *HCE)
{
    //Create a (G4) hit collection for this event
    // hitsColl = new LCHitsCollection(SDName, collectionName[0]);
    hitsColl = new LCHitsCollection(SDName, collName);

    if (HCID < 0) {
      HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collName);
    }

    // Would be sufficient to add the collection at the end of the event, but
    // doing it here suppresses a warning during compilation
    HCE->AddHitsCollection(HCID, hitsColl);

    //Reset the hit map
    hitMap->clear();
}

//Pass the hit collection to the other G4 hits collection
//These collections will be transformed into the LCIO Collection you
//find in the output file
void LCSensitiveDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
    LCHitMapIterator iter;

    // the hit map stores all the hits - they are inserted during ProcessHits()
    // This loop cycles through the hit map and adds them all to hitsColl
    for (iter=hitMap->begin(); iter != hitMap->end(); ++iter) {
        hitsColl->insert(iter->second);
    }

    hitMap->clear(); // hit map has already been copied to hitsColl

    // Add hit collection to HCE
    HCE->AddHitsCollection(HCID, hitsColl);

}


G4bool LCSensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
    // -------------- LOCAL VARIABLES AND CHECKS --------------

    // store energy deposition and momentum change from primary particles
    G4double edep = aStep->GetTotalEnergyDeposit();

        // make sure you aren't using geantinos
        if (edep <= 0 &&
                aStep->GetTrack()->GetDefinition()->GetParticleType() != "geantino") {
            return true;
        }

    // PARTICLE INFORMATION ---------------------------------

    // Get parent ID
    G4int parentID = aStep->GetTrack()->GetParentID();
    // time of flight
    G4double ToF   = aStep->GetTrack()->GetGlobalTime();

     // Set primary particle track ID/PDG
    G4int PDG=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

    if ( parentID <= 0 ){
     G4int PID = aStep->GetTrack()->GetTrackID();
     if( PID != primaryID )
       {
           primaryID = PID;
           primaryPDG= PDG;
       }
     }

    // Get location (volumes & coordinates) with PreStepPoint->TouchableHandle
    const G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    const G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
    G4ThreeVector GlobalHitPos = ( (preStepPoint->GetPosition())+(postStepPoint->GetPosition())) / 2.;
#ifdef LCSD_DEBUG
    G4cout << "GlobalHitPosition " << GlobalHitPos/mm << " mm " <<G4endl;
#endif
    G4int LC_num = 0, layer_num = 0, sector_num = 0, cell_num = 0;
        // Use the touchable handle to get the volume hierarchy for the hit
        G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
            // Find the unique volume path for a cell which has a hit
                //  copy number of LumiCal
             LC_num     = theTouchable->GetHistory()->GetVolume(1)->GetCopyNo();
                // layer inside LumiCal
             layer_num  = theTouchable->GetHistory()->GetVolume(2)->GetCopyNo();

     if ( !VirtualCell ){
       cell_num   = theTouchable->GetReplicaNumber(0);   //     ring
       sector_num = theTouchable->GetReplicaNumber(1);   //     phi sector

#ifdef LCSD_DEBUG
       	     G4cout << " LCAL arm : "  << LC_num 
                    << " Layer:     "  << layer_num
                    << " Sector:    "  << sector_num 
                    << " Cell:      "  << cell_num 
                    << G4endl;
#endif	    		  
     }else{           // virtual cell case - we need to compute ring and sector
	G4ThreeVector LocalHitPos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos) ;

#ifdef LCSD_DEBUG
	G4cout << "LocalHitPosition " << LocalHitPos/mm <<" mm" << G4endl;
#endif 
		  G4double rho = LocalHitPos.getRho();
		  G4double phi = LocalHitPos.getPhi();
		  //
		  //		  G4cout <<"Local rho "<< rho << " phi " << phi /deg << G4endl;
		  //
                  phi = ( phi < 0. ) ? phi + 2.* M_PI : phi; 
                  sector_num = (G4int)floor ( phi / cellDimPhi );
                  cell_num   = (G4int)floor(std::abs( rho - CalRhoMin ) / cellDimRho );
                  cell_num = ( cell_num >NumCellsRho  ) ? NumCellsRho : cell_num; 
#ifdef LCSD_DEBUG
       	     G4cout << " LCAL arm : "  << LC_num 
                    << " Layer:     "  << layer_num
                    << " Sector:    "  << sector_num 
                    << " Cell:      "  << cell_num 
                    << G4endl;
#endif		  		  
     }

    /* ENCODE THE HIT ---------------------------------
     Use bitwise operations to store the location of a cell in a single
     32-bit word
     4 bytes, 4 levels in the volume tree - 1 byte per volume
     | LC number | Layer number | Sector number | Cell number |
     note - this only works because no volume will have a number > 255 (2^8)
    */
    cell_code cellID;
    cellID.id0 = 0; cellID.id1 = 0;   // 32 0's in a row per member
                                      // LumiCal only uses id0; id1 is a legacy from the Mokka version

    cellID.id0 |= (cell_num   <<  0); // store the rcell #   (0-63)
    cellID.id0 |= (sector_num <<  8); // shift the phisector (0-47)
    cellID.id0 |= (layer_num  << 16); // shift the layer #   (1-30)
    cellID.id0 |= (LC_num     << 24); // shift the LumiCal # (1-2)

    // get local and global position using touchable
    // check if the cell has been hit before, and check the indices
    if (!FindHit(cellID.id0, edep, primaryID, PDG) &&
        !( (cell_num > NumCellsRho) || (sector_num > NumSectorsPhi) ) )  {

        // Global hit coordinates

        // FG: the local coordinate system of the cell replica is the same as for the disk
        //     containing the the pad - rotated by phi of the phi replica, i.e.
        //     the origin is at the center of the disk
        //     hence the position of the cell center in its coordinate system is
        //     given by:
      G4ThreeVector localCellPos(CalRhoMin+((G4double)cell_num + 0.5)*cellDimRho,  0. , 0. );
       
      if ( VirtualCell ) localCellPos.setPhi( ((G4double)sector_num +0.5000) * cellDimPhi );
	else             localCellPos.setPhi( 0.5000 * cellDimPhi );
#ifdef LCSD_DEBUG
      G4cout << "Local Cell position " << localCellPos << G4endl;
#endif

        // compute GlobalCellPos based on touchable with localCellPos
        G4ThreeVector GlobalCellPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(localCellPos);

#ifdef LCSD_DEBUG
	G4cout << "Global Cell position " << GlobalCellPos << G4endl;
#endif

        // assert id w/in valid range using bitwise ops
        // (0xff = 255 in hex)
        assert((cellID.id0 & 0xff)<=120);


        // Use all this information to create an LCHit instance
        LCHit *theHit = new LCHit(GlobalHitPos.x(),    // global hit position
                                  GlobalHitPos.y(),    //
                                  GlobalHitPos.z(),     // 
                                  GlobalCellPos.x(),   // GlobalCellPosRho
                                  GlobalCellPos.y(),   // GlobalCellPosPhi
                                  GlobalCellPos.z(),   // GlobalCellPosZ
                                  edep,                // edep
                                  primaryID,           // track number
                                  primaryPDG,          // PDG encoding
                                  cellID,              // Cell ID code
                                  ToF);                // time since event begin
                                                     

        //Insert the hit and cellID into the hitmap
        hitMap->insert(LCHitMapPair(cellID.id0, theHit));
    }

    return true;
}

G4bool LCSensitiveDetector::FindHit(G4int    cellID,
                                    G4double edep,
                                    G4int    trackID,
                                    G4int    PDG)
{
    /*Parameter explanations:
     *  cellID = unique cell identifier
     *  edep = amount of energy being added to the calorimeter cell
     *  TID = track ID of the particle (from Step)
     *  PDG = particle data group identifier code
     */

    //fg: use find method of map !
    // the find method of map returns:
        // successful   - a pointer to the appropriate pair
        // unsuccessful - a pointer to the end marker ( end() )
    LCHitMapIterator iter = hitMap->find(cellID);

    // If you find the hit already in hitMap (as indexed by the cellID),
    // increment its energy!
    if(iter != hitMap->end()) {
         (iter->second)->AddEdep(trackID, PDG, edep) ; 
         return true;
     }
    return false;
}

void LCSensitiveDetector::SetRhoMin(G4double rmin){
    CalRhoMin = rmin;
}
void LCSensitiveDetector::SetPhiOffset(G4double phi0){
    CalPhiOffset = phi0;
}
void LCSensitiveDetector::SetNCellRho(G4int nx )
{
    // set number of cellS in a sector
    NumCellsRho = nx;
    assert(nx > 0 );
}
void LCSensitiveDetector::SetNCellPhi( G4int ny)
{
    // set number of sectors in a layer (= # cells in phi dir)
    NumSectorsPhi = ny;
    assert(ny > 0 );
}
void LCSensitiveDetector::SetRhoCellDim(G4double c1)
{
    cellDimRho = c1;
    assert(cellDimRho > 0 );
}
void LCSensitiveDetector::SetPhiCellDim(G4double c1)
{
    cellDimPhi = c1;
    assert(cellDimPhi > 0 );
}

void LCSensitiveDetector::clear()
{
}

void LCSensitiveDetector::DrawAll()
{
}

void LCSensitiveDetector::PrintAll()
{
}
