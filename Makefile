CC=`root-config --cxx`
LD=`root-config --ld`
CFLAGS=-c -g -fPIC `root-config --cflags`
LDFLAGS=`root-config --glibs` -shared -L$$ALICE_ROOT/lib -L$$ALICE_PHYSICS/lib -lpythia6_4_28 -lpythia8210dev -lAliPythia6 -lAliPythia8 -lTEvtGen -lEvtGen -lSTEERBase -lSTEER -lEVGEN -lESD -lAOD -lANALYSIS -lPWGJEEMCALJetTasks -lPWGEMCALbase -lPWGEMCALtasks -lPWGJETFW -lPWGJEFlavourJetTasks
SOURCES=OnTheFlySimulationGenerator.cxx AliGenEvtGen_dev.cxx
OBJECTS=$(SOURCES:.cxx=.o)
LIBRARY=AnalysisCode.so

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECTS:.o=_Dict.o) -o $@

%.o: %.cxx
	rootcint -f $(@:.o=_Dict.cxx) -c -I`root-config --incdir` -I$$ALICE_ROOT/include $(@:.o=.h)
	$(CC) $(CFLAGS) -I`root-config --incdir` -I$$ALICE_ROOT/include -I$$ALICE_PHYSICS/include -I$$FASTJET/include $(@:.o=_Dict.cxx) $(@:.o=.cxx)
	
clean:
	rm ./*~ ./*.o ./AnalysisCode.so
