CC=`root-config --cxx`
LD=`root-config --ld`
CFLAGS=-c -g -fPIC `root-config --cflags`
LDFLAGS=`root-config --glibs` -shared -L$$ALICE_ROOT/lib -L$$ALICE_PHYSICS/lib -lTHepMCParser -lHepMC -lEG -lEGPythia6 -llhapdfbase -lpythia6_4_28 -lpythia8210dev -lAliPythia6 -lAliPythia8 -lTEvtGen -lEvtGen -lSTEERBase -lSTEER -lEVGEN -lESD -lAOD -lANALYSIS -lPWGJEEMCALJetTasks -lPWGEMCALbase -lPWGEMCALtasks -lPWGJETFW -lPWGJEFlavourJetTasks
SOURCES=OnTheFlySimulationGenerator.cxx AliGenEvtGen_dev.cxx AliGenPythia_dev.cxx AliPythiaBase_dev.cxx AliPythia6_dev.cxx AliPythia8_dev.cxx AliGenExtFile_dev.cxx AliGenReaderHepMC_dev.cxx
OBJECTS=$(SOURCES:.cxx=.o)
LIBRARY=AnalysisCode.so

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECTS:.o=_Dict.o) -o $@

%.o: %.cxx hepmc
	rootcint -f $(@:.o=_Dict.cxx) -c -I`root-config --incdir` -I$$ALICE_ROOT/include -I./ $(@:.o=.h)
	$(CC) $(CFLAGS) -I`root-config --incdir` -I./ -I$$ALICE_ROOT/include -I$$ALICE_PHYSICS/include -I$$FASTJET/include $(@:.o=_Dict.cxx) $(@:.o=.cxx)

hepmc:
	tar -xf HepMC.tar

clean:
	rm  ./*.pcm ./*.o ./AnalysisCode.so ./*Dict*
