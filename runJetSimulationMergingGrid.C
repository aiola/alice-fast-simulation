#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <memory>

#include <TFileMerger.h>
#include <Riostream.h>
#include <TGrid.h>
#include <TString.h>
#include <TObjArray.h>
#include <TAlienCollection.h>

#include <AliLog.h>

std::vector<TString> GetFiles(const TString& strFileList);
std::vector<TString> GetFilesFromTXT(const TString& strFileList);
std::vector<TString> GetFilesFromXML(const TString& strFileList);
std::set<TString> GetFileNames(const std::vector<TString>& file_list);
void AddFilesToMerger(TFileMerger& merger, const TString& file_name_accept, const std::vector<TString>& file_list);

Int_t runJetSimulationMergingGrid(const char* fileList, const char* skipList = "", const char* acceptList = "", Int_t n=2)
{
  TGrid::Connect("alien://");

  TString strFileList(fileList);
  auto file_list = GetFiles(strFileList);

  std::set<TString> file_names = GetFileNames(file_list);

  Int_t merge_errors = 0;

  for (auto fname : file_names) {
    TFileMerger merger(kFALSE);
    merger.OutputFile(fname.Data());
    merger.SetMaxOpenedFiles(n);

    AddFilesToMerger(merger, fname, file_list);

    UInt_t mode = TFileMerger::kAllIncremental;

    TString strSkipList(skipList);
    std::unique_ptr<TObjArray> skipArray(strSkipList.Tokenize(" "));

    TString strAcceptList(acceptList);
    std::unique_ptr<TObjArray> acceptArray(strAcceptList.Tokenize(" "));

    if (skipArray && skipArray->GetEntriesFast() > 0) {
      mode = mode | TFileMerger::kSkipListed;
      if (acceptArray && acceptArray->GetEntriesFast() > 0) {
        AliInfoGeneralStream("runJetSimulationMergingGrid") << "Accept list is being ignored!!!" << std::endl;
      }


      for (auto iter = skipArray->begin(); iter != skipArray->end(); iter.Next()) {
        merger.AddObjectNames((*iter)->GetName());
      }
    }
    else if (acceptArray && acceptArray->GetEntriesFast() > 0) {
      mode = mode | TFileMerger::kOnlyListed;

      for (auto iter = acceptArray->begin(); iter != acceptArray->end(); iter.Next()) {
        merger.AddObjectNames((*iter)->GetName());
      }
    }

    merger.PrintFiles("");
    Int_t r = merger.PartialMerge(mode);

    if (r) {
      AliInfoGeneralStream("runJetSimulationMergingGrid") << "Merge of files '" << fname << "' OK!" << std::endl;
    }
    else {
      AliInfoGeneralStream("runJetSimulationMergingGrid") << "Merge error for files '" << fname << "' OK!" << std::endl;
      merge_errors++;
    }
  }

  return merge_errors;
}

std::vector<TString> GetFiles(const TString& strFileList)
{
  if (strFileList.EndsWith(".xml")) {
    return GetFilesFromXML(strFileList);
  }
  else {
    return GetFilesFromTXT(strFileList);
  }
}

std::vector<TString> GetFilesFromXML(const TString& strFileList)
{
  std::vector<TString> file_list;

  std::unique_ptr<TGridCollection> coll(TAlienCollection::Open(strFileList));
  if (!coll) {
    AliErrorGeneralStream("GetFilesFromXML") <<  "Cannot create an AliEn collection from " << strFileList.Data() << std::endl;
    return file_list;
  }

  coll->Reset();
  while (coll->Next()) {
    TString filename = coll->GetTURL();

    AliInfoGeneralStream("GetFilesFromXML") << "Adding file '" << filename.Data() << "'" << std::endl;
    file_list.push_back(filename.Data());
  }

  AliInfoGeneralStream("GetFilesFromXML") << "Total number of files is " << file_list.size() << std::endl;

  return file_list;
}

std::vector<TString> GetFilesFromTXT(const TString& strFileList)
{
  std::vector<TString> file_list;

  std::ifstream in(strFileList.Data());

  while (in.good()) {
    TString f;

    in >> f;

    if (f.Length() == 0) continue;

    AliInfoGeneralStream("GetFilesFromTXT") << "Adding file '" << f.Data() << "'" << std::endl;

    file_list.push_back(f);
  }

  in.close();

  AliInfoGeneralStream("GetFilesFromTXT") << "Total number of files is " << file_list.size() << std::endl;

  return file_list;
}

void AddFilesToMerger(TFileMerger& merger, const TString& file_name_accept, const std::vector<TString>& file_list)
{
  for (auto file_name : file_list) {
    if (!file_name.EndsWith(file_name_accept)) continue;
    merger.AddFile(file_name);
  }
}

std::set<TString> GetFileNames(const std::vector<TString>& file_list)
{
  std::set<TString> file_names;

  for (auto path : file_list) {
    std::unique_ptr<TObjArray> tree(path.Tokenize("/"));
    file_names.insert(tree->Last()->GetName());
  }

  AliInfoGeneralStream("GetFileNames") << "Found " << file_names.size() << " file names: ";
  for (auto file_name : file_names) {
    std::cout << "'" << file_name.Data() << "' ";
  }
  std::cout << std::endl;

  return file_names;
}
