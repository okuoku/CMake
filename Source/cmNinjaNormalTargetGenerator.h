/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmNinjaNormalTargetGenerator_h
#  define cmNinjaNormalTargetGenerator_h

#  include "cmNinjaTargetGenerator.h"
#  include "cmNinjaTypes.h"

class cmSourceFile;

class cmNinjaNormalTargetGenerator : public cmNinjaTargetGenerator
{
public:
  cmNinjaNormalTargetGenerator(cmTarget* target);
  ~cmNinjaNormalTargetGenerator();

  void Generate();

private:
  std::string LanguageLinkerRule() const;
  const char* GetVisibleTypeName() const;
  void WriteLanguagesRules();
  void WriteLinkRule();
  void WriteLinkStatement();
  std::vector<std::string> ComputeLinkCmd();

private:
  // Target name info.
  std::string TargetNameOut;
  std::string TargetNameSO;
  std::string TargetNameReal;
  std::string TargetNameImport;
  std::string TargetNamePDB;
  const char *TargetLinkLanguage;
};

#endif // ! cmNinjaNormalTargetGenerator_h
