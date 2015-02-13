import FWCore.ParameterSet.Config as cms
process = cms.Process("OMTFEmulation")
import os
import sys
import commands

verbose = False

process.load("FWCore.MessageLogger.MessageLogger_cfi")

if verbose: 
    process.MessageLogger = cms.Service("MessageLogger",
       suppressInfo       = cms.untracked.vstring('AfterSource', 'PostModule'),
       destinations   = cms.untracked.vstring(
                                             'detailedInfo'
                                             ,'critical'
                                             ,'cout'
                    ),
       categories = cms.untracked.vstring(
                                        'CondDBESSource'
                                        ,'EventSetupDependency'
                                        ,'Geometry'
                                        ,'MuonGeom'
                                        ,'GetManyWithoutRegistration'
                                        ,'GetByLabelWithoutRegistration'
                                        ,'Alignment'
                                        ,'SiStripBackPlaneCorrectionDepESProducer'
                                        ,'SiStripLorentzAngleDepESProducer'
                                        ,'SiStripQualityESProducer'
                                        ,'TRACKER'
                                        ,'HCAL'
        ),
       critical       = cms.untracked.PSet(
                        threshold = cms.untracked.string('ERROR') 
        ),
       detailedInfo   = cms.untracked.PSet(
                      threshold  = cms.untracked.string('INFO'), 
                      CondDBESSource  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      EventSetupDependency  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      Geometry  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      MuonGeom  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      Alignment  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      GetManyWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      GetByLabelWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ) 

       ),
       cout   = cms.untracked.PSet(
                threshold  = cms.untracked.string('INFO'), 
                CondDBESSource  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                EventSetupDependency  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                Geometry  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                MuonGeom  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                Alignment  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                GetManyWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                GetByLabelWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ) 
                ),
                                        )

if not verbose:
    process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(50000)
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.source = cms.Source(
    'PoolSource',
    fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEta/721_FullEta_v3/data/SingleMu_16_p_100_3_bhF.root',
                                      'file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEta/721_FullEta_v3/data/SingleMu_16_p_10_1_wJX.root')
    )


##Use all available events in a single job.
##Only for making the connections maps.
process.source.fileNames =  cms.untracked.vstring()
path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEta/721_FullEta_v3/data/"
command = "ls "+path+"/SingleMu_16_*_100_*"
fileList = commands.getoutput(command).split("\n")
process.source.fileNames =  cms.untracked.vstring()
for aFile in fileList:
    process.source.fileNames.append('file:'+aFile)


print process.source.fileNames

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))

###PostLS1 geometry used
process.load('Configuration.Geometry.GeometryExtendedPostLS1Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedPostLS1_cff')
############################
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

path = os.environ['CMSSW_BASE']+"/src/UserCode/OMTFSimulation/data/"


process.load('L1Trigger.L1TMuon.L1TMuonTriggerPrimitiveProducer_cfi')

###OMTF pattern maker configuration
process.omtfPatternMaker = cms.EDAnalyzer("OMTFPatternMaker",
                                      TriggerPrimitiveSrc = cms.InputTag('L1TMuonTriggerPrimitives'),
                                      g4SimTrackSrc = cms.InputTag('g4SimHits'),
                                      makeGoldenPatterns = cms.bool(True),                                     
                                      makeConnectionsMaps = cms.bool(False),                                      
                                      dropRPCPrimitives = cms.bool(False),                                    
                                      dropDTPrimitives = cms.bool(False),                                    
                                      dropCSCPrimitives = cms.bool(False),   
                                      ptCode = cms.int32(16),
                                      charge = cms.int32(1),
                                      omtf = cms.PSet(
        #configXMLFile = cms.string(path+"hwToLogicLayer_18layersFix2.xml"),
        configXMLFile = cms.string(path+"hwToLogicLayer_721.xml"),
        patternsXMLFiles = cms.vstring(path+"Patterns_ipt6_18.xml",path+"Patterns_ipt19_31.xml"),

        )
                                      )

###Gen level filter configuration
process.MuonEtaFilter = cms.EDFilter("SimTrackEtaFilter",
                                minNumber = cms.uint32(1),
                                src = cms.InputTag("g4SimHits"),
                                cut = cms.string("momentum.eta<1.15 && momentum.eta>0.83 &&  momentum.pt>1")
                                )
process.GenMuPath = cms.Path(process.MuonEtaFilter)
##########################################



process.L1TMuonSeq = cms.Sequence( process.L1TMuonTriggerPrimitives+ 
                                   process.omtfPatternMaker)

process.L1TMuonPath = cms.Path(process.MuonEtaFilter*process.L1TMuonSeq)

process.schedule = cms.Schedule(process.GenMuPath,process.L1TMuonPath)

