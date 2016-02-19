// -*- C++ -*-
//
// Package:    DualAnode/DualAnodeDigis
// Class:      DualAnodeDigisProducer
// 
/**\class DualAnodeDigisProducer DualAnodeDigisProducer.cc DualAnode/DualAnodeDigis/plugins/DualAnodeDigisProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Luke Molacek
//         Created:  Thu, 25 Jun 2015 19:24:49 GMT
//
//
//Luke added includes
#include <iostream>
#include <fstream>
#include <string>
#include "DataFormats/HcalDigi/interface/QIE10DataFrame.h"
#include <vector>
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



#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "TRandom.h"
#include <stdlib.h>


//
// class declaration
//

class DualAnodeDigisProducer : public edm::stream::EDProducer<> {
   public:
      explicit DualAnodeDigisProducer(const edm::ParameterSet&);
      ~DualAnodeDigisProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;


      void fillChan(double soi_energy, int noisemode, QIE10DataFrame& df1, QIE10DataFrame& df2);

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      int n_samples;
      double energy;
      int dropmode;
      int noisemode;
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

DualAnodeDigisProducer::DualAnodeDigisProducer(const edm::ParameterSet& iConfig)
{
    n_samples=3;
    energy = iConfig.getParameter<double>("energy");
    dropmode = iConfig.getParameter<int>("dropmode");
    noisemode = iConfig.getParameter<int>("noisemode");
    srand(time(0));

    typedef HcalDataFrameContainer<QIE10DataFrame> QIE10DigiCollection;
    produces<QIE10DigiCollection>();
    produces<double>();
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


DualAnodeDigisProducer::~DualAnodeDigisProducer()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}

//Functions added by Luke for adc production
//
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


//double adc_to_charge(int adc_value)
//{
//    using namespace std;
//    double charge = (charge_lower_bound[adc_value]+charge_upper_bound[adc_value])/2;
//    return charge;
//}


int charge_to_adc(double charge)
{
    using namespace std;
    int adc_value=-1;
    for(int i=0;i<(256);i++)
    {
        if (charge < charge_upper_bound [i])
        {
            if (charge >= charge_lower_bound [i])
                {
                adc_value=i;
                }
        }
    }
    if (adc_value==-1)
    {
	cout << "Oh No! adc_value is still " << adc_value << "!" << endl;
	cout << "The charge is " << charge << endl;
    }
    return adc_value;
}

void adc_random_producer(double energy_soi, int noisemode, int& adc_anode_1, int& adc_anode_2)
{
using namespace std;
    // Control parameters all here
//    const Double_t ENERGY_SLOPE = 10;
    const Double_t PE_PER_GEV = 1.0/2;
    const Double_t RATIO_PE_ANODE_1 = 0.5;
    const Double_t FC_PER_PE = 8;
    const Double_t PED_WIDTH = 3;
    const Double_t PED_MEAN = 9;
    const Double_t NOISE = 500;
    // Random Number Generator
    TRandom* rand=gRandom;
    // Production and calculations
 //   double energy_soi=rand->Exp(ENERGY_SLOPE);
 
    double energy_anode_1 = energy_soi * RATIO_PE_ANODE_1; 
    double energy_anode_2 = energy_soi * (1-RATIO_PE_ANODE_1);
    //mode for adding noise to anode 1
    if(noisemode == 1)
    {
    	energy_anode_1 += rand->Exp(NOISE);
    }
    
    double pe_anode_1 = energy_anode_1 * PE_PER_GEV;
    double pe_anode_2 = energy_anode_2 * PE_PER_GEV;
    double poisson_anode_1=rand->Poisson(pe_anode_1);
    double poisson_anode_2=rand->Poisson(pe_anode_2);
    double fc_anode_1=poisson_anode_1*FC_PER_PE;
    double fc_anode_2=poisson_anode_2*FC_PER_PE;
    double fc_anode_1_with_pedestal = fc_anode_1 + rand->Gaus(PED_MEAN,PED_WIDTH);
    double fc_anode_2_with_pedestal = fc_anode_2 + rand->Gaus(PED_MEAN,PED_WIDTH);
    if (fc_anode_1_with_pedestal < 0)
    {
    	fc_anode_1_with_pedestal = 0;
    }
    if (fc_anode_2_with_pedestal < 0)
    {
    	fc_anode_2_with_pedestal = 0;
    }
    adc_anode_1 = charge_to_adc(fc_anode_1_with_pedestal);
    adc_anode_2 = charge_to_adc(fc_anode_2_with_pedestal);

}

//
// member functions
//



void DualAnodeDigisProducer::fillChan(double soi_energy, int noisemode, QIE10DataFrame& df1, QIE10DataFrame& df2)
{
    //Parameters here
    const Double_t T_NOM = 25;
    const Double_t T_NOM_WIDTH = 2;
    const Double_t T_NOISE = 13;
    const Double_t T_NOISE_WIDTH = 1;
    const Double_t T_NOISE_MIN = -2.5;
    const Double_t T_NOISE_MAX = 2.5;
    //Random Number Generator
    TRandom* rand=gRandom;
    
    int le_tdc_anode_1, le_tdc_anode_2, fe_tdc=0, capid;
    if (noisemode == 1)
    {
    	le_tdc_anode_1 = rand->Gaus(T_NOISE, T_NOISE_WIDTH) + rand->Uniform(T_NOISE_MIN, T_NOISE_MAX);
	le_tdc_anode_2 = rand->Gaus(T_NOM, T_NOM_WIDTH);
    }
    else
    {
    	le_tdc_anode_1 = rand->Gaus(T_NOM, T_NOM_WIDTH);
	le_tdc_anode_2 = le_tdc_anode_1;
    }
    bool soi=false, ok=true;
    for (int i=0;i<n_samples;i++)
    {
        soi=(i==1);
    	int anode_1 = -999, anode_2 = -998;
        if (soi) adc_random_producer(soi_energy,noisemode,anode_1,anode_2);
        else adc_random_producer(0,noisemode,anode_1,anode_2);
	capid = i%4;
	uint16_t isample = i;
	if (anode_1 < 30)
	{
	    df1.setSample(isample, anode_1, 63, fe_tdc, capid, soi, ok);
	}
	else
	{
	    df1.setSample(isample, anode_1, le_tdc_anode_1, fe_tdc, capid, soi, ok);
	}
	if (anode_2 < 30)
	{
	    df2.setSample(isample, anode_2, 63, fe_tdc, capid, soi, ok);
	}
	else
	{
	    df2.setSample(isample, anode_2, le_tdc_anode_2, fe_tdc, capid, soi, ok);
	}
    }
}


// ------------ method called to produce the data  ------------
void
DualAnodeDigisProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    std::auto_ptr<QIE10DigiCollection> mystuff( new QIE10DigiCollection(n_samples) );
    std::auto_ptr<double> otherstuff( new double(energy));
 
    QIE10DigiCollection allDigis(n_samples);

    allDigis.addDataFrame(HcalDetId(HcalForward,30,1,1));
    allDigis.addDataFrame(HcalDetId(HcalForward,30,1,3));
    

    QIE10DataFrame df1, df2;
    df1 = allDigis.nextToBack();
    df2 = allDigis.back();

    fillChan(energy,noisemode,df1,df2);

    //mode 1
    if (dropmode == 1) //add both as normal
    {
    	mystuff->addDataFrame(df1);
    	mystuff->addDataFrame(df2);
    }
    // mode 2 randomly drops 1 of the anodes
    else if (dropmode == 2)
    {
    	int drop = rand()%2+1;
	if (drop == 1)
	{
	    mystuff->addDataFrame(df2);
	}
	else if (drop == 2)
	{
	    mystuff->addDataFrame(df1);
	}
    }

    iEvent.put(otherstuff);
    iEvent.put( mystuff);


/* This is an event example
   //Read 'ExampleData' from the Event
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
	
   //Use the ExampleData to create an ExampleData2 which 
   // is put into the Event
   std::unique_ptr<ExampleData2> pOut(new ExampleData2(*pIn));
   iEvent.put(std::move(pOut));
*/

/* this is an EventSetup example
   //Read SetupData from the SetupRecord in the EventSetup
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
*/
 
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
DualAnodeDigisProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
DualAnodeDigisProducer::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
DualAnodeDigisProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
DualAnodeDigisProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
DualAnodeDigisProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
DualAnodeDigisProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DualAnodeDigisProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DualAnodeDigisProducer);
