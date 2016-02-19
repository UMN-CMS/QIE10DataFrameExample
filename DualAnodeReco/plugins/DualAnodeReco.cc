// -*- C++ -*-
//
// Package:    DualAnode/DualAnodeReco
// Class:      DualAnodeReco
// 
/**\class DualAnodeReco DualAnodeReco.cc DualAnode/DualAnodeReco/plugins/DualAnodeReco.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Luke Molacek
//         Created:  Fri, 24 Jul 2015 17:33:55 GMT
//
//


#include <iostream>


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"


//
// class declaration
//

class DualAnodeReco : public edm::stream::EDProducer<> {
   public:
      explicit DualAnodeReco(const edm::ParameterSet&);
      ~DualAnodeReco();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;


      // ----------member data ---------------------------
      struct ChanHits {
          ChanHits(const HcalDetId& anid) : id(anid) {}
          HcalDetId id;
          QIE10DataFrame anode1, anode2;
          void addHit(QIE10DataFrame df) { if (anode1.detid().null()) anode1=df; else if (anode2.detid().null()) anode2=df; }
          int nhits() const { if (!anode2.detid().null()) return 2; 
                        if (!anode1.detid().null()) return 1; 
                        return 0;
                      }
      };
      HFRecHit makeHit(const ChanHits& digis, int which_anode);
      double energyForDigi(QIE10DataFrame df);

      std::map<HcalDetId,ChanHits> m_hitColl;
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
DualAnodeReco::DualAnodeReco(const edm::ParameterSet& iConfig)
{

   produces<HFRecHitCollection>("bothanodeswithcut");
   produces<HFRecHitCollection>("anode1");
   produces<HFRecHitCollection>("anode2");
   produces<HFRecHitCollection>("bothanodesnocut");
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
 
   //if you want to put into the Run
   produces<ExampleData2,InRun>();
*/
   //now do what ever other initialization is needed
  
}


DualAnodeReco::~DualAnodeReco()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}

const double charge_lower_bound[256]={0, 3.2, 6.4, 9.6, 12.8, 16, 19.2, 22.4,
	25.6, 28.8, 32, 35.2, 38.4, 41.6, 44.8, 48, 54.4, 60.8, 67.2, 73.6, 80, 86.4,
	92.8, 99.2, 105.6, 112, 118.4, 124.8, 131.2, 137.6, 144, 150.4, 156.8, 163.2,
	169.6, 176, 188.8, 201.6, 214.4, 227.2, 240, 252.8, 265.6, 278.4, 291.2, 304,
	316.8, 329.6, 342.4, 355.2, 368, 380.8, 393.6, 406.4, 419.2, 432, 444.8, 470.4,
	496, 521.6, 547.2, 572.8, 598.4, 624, 624, 649.6, 675.2, 700.8, 726.4, 752,
	777.6, 803.2, 828.8, 854.4, 880, 905.6, 931.2, 956.8, 982.4, 1008, 1059.2,
	1110.4, 1161.6, 1212.8, 1264, 1315.2, 1366.4, 1417.6, 1468.8, 1520, 1571.2,
	1622.4, 1673.6, 1724.8, 1776, 1827.2, 1878.4, 1929.6, 1980.8, 2032, 2134.4,
	2236.8, 2339.2, 2441.6, 2544, 2646.4, 2748.8, 2851.2, 2953.6, 3056, 3158.4,
	3260.8, 3363.2, 3465.6, 3568, 3670.4, 3772.8, 3875.2, 3977.6, 4080, 4182.4,
	4387.2, 4592, 4796.8, 5001.6, 5206.4, 5411.2, 5616, 5616, 5820.8, 6025.6,
	6230.4, 6435.2, 6640, 6844.8, 7049.6, 7254.4, 7459.2, 7664, 7868.8, 8073.6,
	8278.4, 8483.2, 8688, 9097.6, 9507.2, 9916.8, 10326.4, 10736, 11145.6, 11555.2,
	11964.8, 12374.4, 12784, 13193.6, 13603.2, 14012.8, 14422.4, 14832, 15241.6,
	15651.2, 16060.8, 16470.4, 16880, 17699.2, 18518.4, 19337.6, 20156.8, 20976,
	21795.2, 22614.4, 23433.6, 24252.8, 25072, 25891.2, 26710.4, 27529.6, 28348.8,
	29168, 29987.2, 30806.4, 31625.6, 32444.8, 33264, 34083.2, 35721.2, 37359.2,
	38997.2, 40635.2, 42273.2, 43911.2, 45549.2, 45549.2, 47187.2, 48825.2,
	50463.2, 52101.2, 53739.2, 55377.2, 57015.2, 58653.2, 60291.2, 61929.2,
	63567.2, 65205.2, 66843.2, 68481.2, 70119.2, 73395.2, 76671.2, 79947.2,
	83223.2, 86499.2, 89775.2, 93051.2, 96327.2, 99603.2, 102879.2, 106155.2,
	109431.2, 112707.2, 115983.2, 119259.2, 122535.2, 125811.2, 129087.2, 132363.2,
	135639.2, 142191.2, 148743.2, 155295.2, 161847.2, 168399.2, 174951.2, 181503.2,
	188055.2, 194607.2, 201159.2, 207711.2, 214263.2, 220815.2, 227367.2, 233919.2,
	240471.2, 247023.2, 253575.2, 260127.2, 266679.2, 273231.2, 286331.2, 299431.2,
	312531.2, 325631.2, 338731.2, 351831.2, 364931.2};
const double charge_upper_bound[256]={3.2, 6.4, 9.6, 12.8, 16, 19.2, 22.4,
	25.6, 28.8, 32, 35.2, 38.4, 41.6, 44.8, 48, 54.4, 60.8, 67.2, 73.6, 80, 86.4,
	92.8, 99.2, 105.6, 112, 118.4, 124.8, 131.2, 137.6, 144, 150.4, 156.8, 163.2,
	169.6, 176, 188.8, 201.6, 214.4, 227.2, 240, 252.8, 265.6, 278.4, 291.2, 304,
	316.8, 329.6, 342.4, 355.2, 368, 380.8, 393.6, 406.4, 419.2, 432, 444.8, 470.4,
	496, 521.6, 547.2, 572.8, 598.4, 624, 624, 649.6, 675.2, 700.8, 726.4, 752,
	777.6, 803.2, 828.8, 854.4, 880, 905.6, 931.2, 956.8, 982.4, 1008, 1059.2,
	1110.4, 1161.6, 1212.8, 1264, 1315.2, 1366.4, 1417.6, 1468.8, 1520, 1571.2,
	1622.4, 1673.6, 1724.8, 1776, 1827.2, 1878.4, 1929.6, 1980.8, 2032, 2134.4,
	2236.8, 2339.2, 2441.6, 2544, 2646.4, 2748.8, 2851.2, 2953.6, 3056, 3158.4,
	3260.8, 3363.2, 3465.6, 3568, 3670.4, 3772.8, 3875.2, 3977.6, 4080, 4182.4,
	4387.2, 4592, 4796.8, 5001.6, 5206.4, 5411.2, 5616, 5616, 5820.8, 6025.6,
	6230.4, 6435.2, 6640, 6844.8, 7049.6, 7254.4, 7459.2, 7664, 7868.8, 8073.6,
	8278.4, 8483.2, 8688, 9097.6, 9507.2, 9916.8, 10326.4, 10736, 11145.6, 11555.2,
	11964.8, 12374.4, 12784, 13193.6, 13603.2, 14012.8, 14422.4, 14832, 15241.6,
	15651.2, 16060.8, 16470.4, 16880, 17699.2, 18518.4, 19337.6, 20156.8, 20976,
	21795.2, 22614.4, 23433.6, 24252.8, 25072, 25891.2, 26710.4, 27529.6, 28348.8,
	29168, 29987.2, 30806.4, 31625.6, 32444.8, 33264, 34083.2, 35721.2, 37359.2,
	38997.2, 40635.2, 42273.2, 43911.2, 45549.2, 45549.2, 47187.2, 48825.2,
	50463.2, 52101.2, 53739.2, 55377.2, 57015.2, 58653.2, 60291.2, 61929.2,
	63567.2, 65205.2, 66843.2, 68481.2, 70119.2, 73395.2, 76671.2, 79947.2,
	83223.2, 86499.2, 89775.2, 93051.2, 96327.2, 99603.2, 102879.2, 106155.2,
	109431.2, 112707.2, 115983.2, 119259.2, 122535.2, 125811.2, 129087.2, 132363.2,
	135639.2, 142191.2, 148743.2, 155295.2, 161847.2, 168399.2, 174951.2, 181503.2,
	188055.2, 194607.2, 201159.2, 207711.2, 214263.2, 220815.2, 227367.2, 233919.2,
	240471.2, 247023.2, 253575.2, 260127.2, 266679.2, 273231.2, 286331.2, 299431.2,
	312531.2, 325631.2, 338731.2, 351831.2, 364931.2, 378031.2};


double adc_to_charge(int adc_value)
{
    using namespace std;
    double charge = (charge_lower_bound[adc_value]+charge_upper_bound[adc_value])/2;
    return charge;
}


HFRecHit DualAnodeReco::makeHit(const ChanHits& digis, int which_anode)
{
using namespace std;
//both anodes with cut
if (which_anode == 3)
{
    
    if (digis.nhits() == 1)
    {
	int isample=100;
	for (int i=0;i<digis.anode1.samples();i++)
	{
	    if (digis.anode1[i].soi())
	    {
	    	isample = i;
	    }
	}
	float timerising;
	if (digis.anode1[isample].le_tdc() >= 0 && digis.anode1[isample].le_tdc() <=49)
	{
	    timerising = digis.anode1[isample].le_tdc()*0.5;
	}
	else
	{
	    timerising = -1000;
	}
	double energy = energyForDigi(digis.anode1)*2;
	//Applying cut for under 10 ns
	if (timerising < 10)
	{
	    return HFRecHit(digis.id,0,timerising);
	}
	else
	{
	    return HFRecHit(digis.id,energy,timerising);
    	}
    }
    else if (digis.nhits() == 2)
    {
    	int isample1=100;
	int isample2=100;
	for (int i=0;i<digis.anode1.samples();i++)
	{
	    if (digis.anode1[i].soi())
	    {
	    	isample1=i;
	    }
	}
	for (int i=0;i<digis.anode2.samples();i++)
	{
	    if (digis.anode2[i].soi())
	    {
	    	isample2=i;
	    }
	}
	float timerising_anode_1, timerising_anode_2, timerising;
	if (digis.anode1[isample1].le_tdc() >= 0 && digis.anode1[isample1].le_tdc() <=49 && digis.anode2[isample2].le_tdc() >= 0 && digis.anode2[isample2].le_tdc() <=49)
	{
	    timerising_anode_1 = digis.anode1[isample1].le_tdc()*0.5;
	    timerising_anode_2 = digis.anode2[isample2].le_tdc()*0.5;
	    timerising = (timerising_anode_1 + timerising_anode_2)/2;    
	}
	else
	{
	    timerising = -1000;
	    timerising_anode_1 = -1000;
	    timerising_anode_2 = -1000;
	}
	double energy;
	//Applying cut for under 10 ns
	//You have not accounted for the case where both times are under 10 ns!!
	if (timerising_anode_1 < 10)
	{
	    energy = energyForDigi(digis.anode2)*2;
	    return HFRecHit(digis.id,energy,timerising_anode_2);
	}
	else if (timerising_anode_2 < 10)
	{
	    energy = energyForDigi(digis.anode1)*2;
	    return HFRecHit(digis.id,energy,timerising_anode_1);
    	}
	else
	{
	    energy = energyForDigi(digis.anode1) + energyForDigi(digis.anode2);
	    return HFRecHit(digis.id,energy,timerising);
	}
    }
    else //This should never be able to happen, must be here to compile
    {
    	return HFRecHit(digis.id,0,0);
    }
}
// both anodes no cut
if (which_anode == 4)
{
    
    if (digis.nhits() == 1)
    {
	int isample=100;
	for (int i=0;i<digis.anode1.samples();i++)
	{
	    if (digis.anode1[i].soi())
	    {
	    	isample = i;
	    }
	}
	float timerising;
	if (digis.anode1[isample].le_tdc() >= 0 && digis.anode1[isample].le_tdc() <=49)
	{
	    timerising = digis.anode1[isample].le_tdc()*0.5;
	}
	else
	{
	    timerising = -1000;
	}
	double energy = energyForDigi(digis.anode1)*2;
        return HFRecHit(digis.id,energy,timerising);
    }
    else if (digis.nhits() == 2)
    {
    	int isample1=100;
	int isample2=100;
	for (int i=0;i<digis.anode1.samples();i++)
	{
	    if (digis.anode1[i].soi())
	    {
	    	isample1=i;
	    }
	}
	for (int i=0;i<digis.anode2.samples();i++)
	{
	    if (digis.anode2[i].soi())
	    {
	    	isample2=i;
	    }
	}
	float timerising_anode_1, timerising_anode_2, timerising;
	if (digis.anode1[isample1].le_tdc() >= 0 && digis.anode1[isample1].le_tdc() <=49 && digis.anode2[isample2].le_tdc() >= 0 && digis.anode2[isample2].le_tdc() <=49)
	{
	    timerising_anode_1 = digis.anode1[isample1].le_tdc()*0.5;
	    timerising_anode_2 = digis.anode2[isample2].le_tdc()*0.5;
	    timerising = (timerising_anode_1 + timerising_anode_2)/2;    
	}
	else
	{
	    timerising = -1000;
	    timerising_anode_1 = -1000;
	    timerising_anode_2 = -1000;
	}
	double energy;
	energy = energyForDigi(digis.anode1) + energyForDigi(digis.anode2);
	return HFRecHit(digis.id,energy,timerising);
    }
    else //This should never be able to happen, must be here to compile
    {
    	return HFRecHit(digis.id,0,0);
    }
}

    else if (which_anode == 1)
    {
	int isample=100;
	for (int i=0;i<digis.anode1.samples();i++)
	{
	    if (digis.anode1[i].soi())
	    {
	    	isample = i;
	    }
	}
	float timerising;
	if (digis.anode1[isample].le_tdc() >= 0 && digis.anode1[isample].le_tdc() <=49)
	{
	    timerising = digis.anode1[isample].le_tdc()*0.5;
	}
	else
	{
	    timerising = -1000;
	}
	double energy = energyForDigi(digis.anode1)*2;
	//For now will comment out because not sure if cuts should be performed here
	//Applying cut for under 10 ns 
//	if (timerising < 10)
//	{
//	    return HFRecHit(digis.id,0,timerising);
//	}
//	else
//	{
	    return HFRecHit(digis.id,energy,timerising);
//    	}
    }
    else if (which_anode == 2)
    {
	int isample=100;
	for (int i=0;i<digis.anode2.samples();i++)
	{
	    if (digis.anode2[i].soi())
	    {
	    	isample = i;
	    }
	}
	float timerising;
	if (digis.anode2[isample].le_tdc() >= 0 && digis.anode2[isample].le_tdc() <=49)
	{
	    timerising = digis.anode2[isample].le_tdc()*0.5;
	}
	else
	{
	    timerising = -1000;
	}
	double energy = energyForDigi(digis.anode2)*2;
	//For now will comment out because not sure if cuts should be performed here
	//Applying cut for under 10 ns
//	if (timerising < 10)
//	{
//	    return HFRecHit(digis.id,0,timerising);
//	}
//	else
//	{
	    return HFRecHit(digis.id,energy,timerising);
//	}
    }
    else
    {
    	return HFRecHit(digis.id,0,0);
    }
}


double DualAnodeReco::energyForDigi(QIE10DataFrame df)
{
    int isample=100;
    for (int i=0;i<df.samples();i++)
    {
    	if (df[i].soi())
	{
	    isample = i;
	}
    }
    int adc_value = df[isample].adc();
    const Double_t PED_MEAN = 9;
    double charge = adc_to_charge(adc_value);
    double charge_min_ped = charge - PED_MEAN;
    const Double_t PE_PER_GEV = 1.0/2;
    const Double_t FC_PER_PE = 8;
    double energy = charge_min_ped/(FC_PER_PE*PE_PER_GEV);
    return energy;
    
}




//
// member functions
//

// ------------ method called to produce the data  ------------
void
DualAnodeReco::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   Handle<QIE10DigiCollection> pDigis;
   iEvent.getByLabel("digimaker",pDigis);

/* This is an event example
   //Read 'ExampleData' from the Event
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);

   //Use the ExampleData to create an ExampleData2 which 
   // is put into the Event
*/
   m_hitColl.clear();

   for (int i=0; i<pDigis->size(); i++) {
     // make official id
     HcalDetId id=pDigis->at(i).detid();
     if (id.depth()>2) id=HcalDetId(id.subdet(),id.ieta(),id.iphi(),id.depth()-2);

     // look to see if we have a hit already
     std::map<HcalDetId,ChanHits>::iterator j=m_hitColl.find(id);
     if (j==m_hitColl.end()) {
        std::pair<std::map<HcalDetId,ChanHits>::iterator, bool> iinsert=m_hitColl.insert(std::pair<HcalDetId,ChanHits>(id,ChanHits(id)));
        j=iinsert.first;
     }
     // one way or another, j points to where to put the digi now
     j->second.addHit(pDigis->at(i));
   }
   std::unique_ptr<HFRecHitCollection> pOut_both_anodes_with_cut(new HFRecHitCollection);
   std::unique_ptr<HFRecHitCollection> pOut_anode_1(new HFRecHitCollection);
   std::unique_ptr<HFRecHitCollection> pOut_anode_2(new HFRecHitCollection);
   std::unique_ptr<HFRecHitCollection> pOut_both_anodes_no_cut(new HFRecHitCollection);

   // now we make the hits
   for (std::map<HcalDetId,ChanHits>::const_iterator j=m_hitColl.begin(); j!=m_hitColl.end(); j++) {
 	pOut_both_anodes_with_cut->push_back(makeHit(j->second, 3));
 	pOut_anode_1->push_back(makeHit(j->second, 1));
 	pOut_anode_2->push_back(makeHit(j->second, 2));
	pOut_both_anodes_no_cut->push_back(makeHit(j->second,4));

   }
   iEvent.put(std::move(pOut_both_anodes_with_cut), "bothanodeswithcut");
   iEvent.put(std::move(pOut_anode_1), "anode1");
   iEvent.put(std::move(pOut_anode_2), "anode2");
   iEvent.put(std::move(pOut_both_anodes_no_cut), "bothanodesnocut");

   m_hitColl.clear();

/* this is an EventSetup example
   //Read SetupData from the SetupRecord in the EventSetup
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
*/
 
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
DualAnodeReco::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
DualAnodeReco::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
DualAnodeReco::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
DualAnodeReco::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
DualAnodeReco::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
DualAnodeReco::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DualAnodeReco::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DualAnodeReco);
