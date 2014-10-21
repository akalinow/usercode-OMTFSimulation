import FWCore.ParameterSet.Config as cms
process = cms.Process("MakePatterns")
import os
import sys

#inputFiles =  cms.vstring("/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/Tree/v1/data//l1RpcTree_p19_p_v1_FullEta_1_1_mqa.root")
inputFiles =  cms.vstring("/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/Tree/v1/data//l1RpcTree_p10_p_v1_FullEta_1_1_Ylg.root")

#inputFiles =  cms.vstring("/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/Tree/v1/data//l1RpcTree_p6_p_v1_FullEta_1_1_hdg.root",
#                          #"/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/Tree/v1/data//l1RpcTree_p6_m_v1_FullEta_1_1_YpO.root"
#                         )

process.load("FWCore.MessageLogger.MessageLogger_cfi")
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
                      Alignment  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
       ),
       cout   = cms.untracked.PSet(
                threshold  = cms.untracked.string('INFO'), 
                CondDBESSource  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                EventSetupDependency  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                Geometry  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                Alignment  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
       ),
)
#process.MessageLogger = cms.Service("MessageLogger")

process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS162_V1::All'
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')

path = os.environ['CMSSW_BASE']+"/src/UserCode/OMTFSimulation/data/"

process.omtfAnalysis = cms.EDAnalyzer("OMTFROOTReader",
  treeFileNames = cms.vstring(inputFiles),
  maxEvents =  cms.int32(10000), #FIXME
  dumpResultToXML = cms.bool(False),                                     
  dumpGPToXML = cms.bool(False),                                     
  makeConnectionsMaps = cms.bool(False),                                      
  omtf = cms.PSet(
    configXMLFile = cms.string(path+"hwToLogicLayer.xml"),
    patternsXMLFiles = cms.vstring(path+"Patterns_chPlus.xml",path+"Patterns_chMinus.xml"),
 ),
 filterByAnaSiMuDistribution = cms.bool(True),
 anaSiMuDistribution = cms.PSet(
   ptMin = cms.double(2.),
   ptMax = cms.double(1210.),
   absEtaMinRef = cms.double(0.83),
   absEtaMaxRef  = cms.double(1.24),
   phiMinRef = cms.double(-3.2),
   phiMaxRef = cms.double(3.2),
 ),
  anaEff      = cms.PSet( maxDR= cms.double(9999.)),
)

process.p = cms.Path(process.omtfAnalysis)


