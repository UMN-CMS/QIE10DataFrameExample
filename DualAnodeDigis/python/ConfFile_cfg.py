import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("EmptySource")
process.load("FWCore.MessageService.MessageLogger_cfi")
#process.load("Configuration.Geometry.GeometryIdeal_cff")
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = 'MCRUN2_74_V8A'

process.digimaker = cms.EDProducer('DualAnodeDigisProducer',
			energy = cms.double(500.0),
			dropmode = cms.int32(1), #1 is normal fill both, 2 is randomly drop 1
			noisemode = cms.int32(1) # 1 gives noise to anode 1, 0 is off
			)

process.hitmaker = cms.EDProducer('DualAnodeReco')

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('DigisE500dropmode1noisemode110nscut.root')
)

process.dump  = cms.EDAnalyzer("HcalDigiDump")
process.dump2 = cms.EDAnalyzer("HcalRecHitDump")




process.p = cms.Path(process.digimaker * process.dump * process.hitmaker *process.dump2)

process.e = cms.EndPath(process.out)
