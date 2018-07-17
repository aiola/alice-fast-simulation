def GetPDFName(lhapdf_id, add_extension=True):
    if lhapdf_id == 11000:
        pdfname = "CT10nlo"
    elif lhapdf_id == 10550:
        pdfname = "cteq66"
    elif lhapdf_id == 10042:
        pdfname = "CTEQ6L1"
    else:
        print("LHAPDF ID {} not known.".format(lhapdf_id))
        exit(1)

    if add_extension:
        pdfname += ".LHgrid"
    return pdfname
