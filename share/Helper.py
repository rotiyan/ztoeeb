###################### Helper functions###############
# figure out if file is data or MC ---------------------------------------------
def IsData():

    from RecExConfig.InputFilePeeker import inputFileSummary

    if inputFileSummary['evt_type'][0]   == "IS_DATA":
        globalflags.DataSource = 'data'
        print "Processing data"
        return True

    elif inputFileSummary['evt_type'][0] == "IS_SIMULATION":
        globalflags.DataSource = 'geant4'
        print "Processing MC"
        return False

    else:
        print "WARNING: Unknown evt_type:", inputFileSummary['evt_type'][0]
#-------------------------------------------------------------------------------
