/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmLocalNinjaGenerator.h"
#include "cmCustomCommandGenerator.h"
#include "cmMakefile.h"
#include "cmGlobalNinjaGenerator.h"
#include "cmNinjaTargetGenerator.h"
#include "cmGeneratedFileStream.h"
#include "cmSourceFile.h"
#include "cmComputeLinkInformation.h"
#include "cmake.h"

#include <assert.h>

cmLocalNinjaGenerator::cmLocalNinjaGenerator()
  : cmLocalGenerator()
  , ConfigName("")
  , HomeRelativeOutputPath("")
{
  // TODO(Nicolas Despres): Maybe I should set this one to true??
  this->IsMakefileGenerator = false;
}

//----------------------------------------------------------------------------
// Virtual public methods.

cmLocalNinjaGenerator::~cmLocalNinjaGenerator()
{
}

void cmLocalNinjaGenerator::Generate()
{
  this->SetConfigName();

  this->WriteProcessedMakefile(this->GetBuildFileStream());
  this->WriteProcessedMakefile(this->GetRulesFileStream());

  this->WriteBuildFileTop();

  cmTargets& targets = this->GetMakefile()->GetTargets();
  for(cmTargets::iterator t = targets.begin(); t != targets.end(); ++t)
    {
    cmNinjaTargetGenerator* tg = cmNinjaTargetGenerator::New(&t->second);
    if(tg)
      {
      tg->Generate();
      // Add the target to "all" if required.
      if (!this->GetGlobalNinjaGenerator()->IsExcluded(
            this->GetGlobalNinjaGenerator()->LocalGenerators[0],
            t->second))
        this->AddDependencyToAll(tg->GetTargetName());
      delete tg;
      }
    }

  this->WriteCustomCommandBuildStatements();
}

// Implemented in:
//   cmLocalUnixMakefileGenerator3.
// Used in:
//   Source/cmMakefile.cxx
//   Source/cmGlobalGenerator.cxx
void cmLocalNinjaGenerator::Configure()
{
  // Compute the path to use when referencing the current output
  // directory from the top output directory.
  this->HomeRelativeOutputPath =
    this->Convert(this->Makefile->GetStartOutputDirectory(), HOME_OUTPUT);
  if(this->HomeRelativeOutputPath == ".")
    {
    this->HomeRelativeOutputPath = "";
    }
  this->cmLocalGenerator::Configure();

}

// TODO(Nicolas Despres): Picked up from cmLocalUnixMakefileGenerator3.
// Refactor it.
std::string cmLocalNinjaGenerator
::GetTargetDirectory(cmTarget const& target) const
{
  std::string dir = cmake::GetCMakeFilesDirectoryPostSlash();
  dir += target.GetName();
#if defined(__VMS)
  dir += "_dir";
#else
  dir += ".dir";
#endif
  return dir;
}

//----------------------------------------------------------------------------
// Non-virtual public methods.

cmGlobalNinjaGenerator* cmLocalNinjaGenerator::GetGlobalNinjaGenerator() const
{
  return static_cast<cmGlobalNinjaGenerator*>(this->GetGlobalGenerator());
}

// TODO(Nicolas Despres): Picked up from cmLocalUnixMakefileGenerator3.
// Refactor it.
std::string
cmLocalNinjaGenerator
::GetObjectFileName(const cmTarget& target,
                    const cmSourceFile& source)
{
  // Make sure we never hit this old case.
  if(source.GetProperty("MACOSX_PACKAGE_LOCATION"))
    {
    std::string msg = "MACOSX_PACKAGE_LOCATION set on source file: ";
    msg += source.GetFullPath();
    this->GetMakefile()->IssueMessage(cmake::INTERNAL_ERROR,
                                      msg.c_str());
    }

  // Start with the target directory.
  std::string obj = this->GetTargetDirectory(target);
  obj += "/";

  // Get the object file name without the target directory.
  std::string dir_max;
  dir_max += this->Makefile->GetCurrentOutputDirectory();
  dir_max += "/";
  dir_max += obj;
  std::string objectName =
    this->GetObjectFileNameWithoutTarget(source, dir_max, 0);
  // Append the object name to the target directory.
  obj += objectName;
  return obj;
}

//----------------------------------------------------------------------------
// Virtual protected methods.

std::string cmLocalNinjaGenerator::ConvertToLinkReference(std::string const& lib)
{
  return this->Convert(lib.c_str(), HOME_OUTPUT, SHELL);
}

std::string
cmLocalNinjaGenerator::ConvertToIncludeReference(std::string const& path)
{
  return this->Convert(path.c_str(), HOME_OUTPUT, SHELL);
}

//----------------------------------------------------------------------------
// Private methods.

cmGeneratedFileStream& cmLocalNinjaGenerator::GetBuildFileStream() const
{
  return *this->GetGlobalNinjaGenerator()->GetBuildFileStream();
}

cmGeneratedFileStream& cmLocalNinjaGenerator::GetRulesFileStream() const
{
  return *this->GetGlobalNinjaGenerator()->GetRulesFileStream();
}

cmake* cmLocalNinjaGenerator::GetCMakeInstance() const
{
  return this->GetGlobalGenerator()->GetCMakeInstance();
}

bool cmLocalNinjaGenerator::isRootMakefile() const
{
  return (strcmp(this->Makefile->GetCurrentDirectory(),
                 this->GetCMakeInstance()->GetHomeDirectory()) == 0);
}

void cmLocalNinjaGenerator::WriteBuildFileTop()
{
  // We do that only once for the top CMakeLists.txt file.
  if(!this->isRootMakefile())
    return;

  // For the build file.
  this->WriteProjectHeader(this->GetBuildFileStream());
  this->WriteNinjaFilesInclusion(this->GetBuildFileStream());

  // For the rule file.
  this->WriteProjectHeader(this->GetRulesFileStream());
}

void cmLocalNinjaGenerator::WriteProjectHeader(std::ostream& os)
{
  cmGlobalNinjaGenerator::WriteDivider(os);
  os
    << "# Project: " << this->GetMakefile()->GetProjectName() << std::endl
    << "# Configuration: " << this->ConfigName << std::endl
    ;
  cmGlobalNinjaGenerator::WriteDivider(os);
}

void cmLocalNinjaGenerator::WriteNinjaFilesInclusion(std::ostream& os)
{
  cmGlobalNinjaGenerator::WriteDivider(os);
  os
    << "# Include auxiliary files.\n"
    << "\n"
    ;
  cmGlobalNinjaGenerator::WriteInclude(os,
                                       cmGlobalNinjaGenerator::NINJA_RULES_FILE,
                                       "Include rules file.");
  os << "\n";
}

void cmLocalNinjaGenerator::SetConfigName()
{
  // Store the configuration name that will be generated.
  if(const char* config = this->GetMakefile()->GetDefinition("CMAKE_BUILD_TYPE"))
    {
    // Use the build type given by the user.
    this->ConfigName = config;
    }
  else
    {
    // No configuration type given.
    this->ConfigName = "";
    }
}

void cmLocalNinjaGenerator::AddDependencyToAll(const std::string& dependency)
{
  this->GetGlobalNinjaGenerator()->AddDependencyToAll(dependency);
}

void cmLocalNinjaGenerator::WriteProcessedMakefile(std::ostream& os)
{
  cmGlobalNinjaGenerator::WriteDivider(os);
  os
    << "# Write statements declared in CMakeLists.txt:" << std::endl
    << "# " << this->Makefile->GetCurrentListFile() << std::endl
    ;
  if(this->isRootMakefile())
    os << "# Which is the root file." << std::endl;
  cmGlobalNinjaGenerator::WriteDivider(os);
  os << std::endl;
}

std::string cmLocalNinjaGenerator::ConvertToNinjaPath(const char *path)
{
  return this->Convert(path,
                       cmLocalGenerator::HOME_OUTPUT,
                       cmLocalGenerator::MAKEFILE);
}

void
cmLocalNinjaGenerator
::AppendTargetOutputs(cmTarget* target, cmNinjaDeps& outputs) {
  switch (target->GetType()) {
  case cmTarget::EXECUTABLE:
  case cmTarget::SHARED_LIBRARY:
  case cmTarget::STATIC_LIBRARY: {
    std::string name = target->GetFullName(this->GetConfigName());
    std::string dir = target->GetDirectory(this->GetConfigName());
    std::string path = ConvertToNinjaPath(dir.c_str());
    if (path.empty() || path == ".")
      outputs.push_back(name);
    else {
      path += "/";
      path += name;
      outputs.push_back(path);
    }
    break;
  }

  case cmTarget::UTILITY:
    outputs.push_back(target->GetName());
    break;
  }
}

void
cmLocalNinjaGenerator
::AppendTargetDepends(cmTarget* target, cmNinjaDeps& outputs) {
  cmTargetDependSet const& targetDeps =
    this->GetGlobalGenerator()->GetTargetDirectDepends(*target);
  for (cmTargetDependSet::const_iterator i = targetDeps.begin();
       i != targetDeps.end(); ++i) {
    this->AppendTargetOutputs(*i, outputs);
  }
}

void cmLocalNinjaGenerator::AppendCustomCommandDeps(const cmCustomCommand *cc, cmNinjaDeps &ninjaDeps) {
  const std::vector<std::string> &deps = cc->GetDepends();
  for (std::vector<std::string>::const_iterator i = deps.begin();
       i != deps.end(); ++i) {
    std::string dep;
    if (this->GetRealDependency(i->c_str(), this->GetConfigName(), dep))
      ninjaDeps.push_back(ConvertToNinjaPath(dep.c_str()));
  }
}

std::string cmLocalNinjaGenerator::BuildCommandLine(const std::vector<std::string> &cmdLines) {
  // If we have no commands but we need to build a command anyway, use ":".
  // This happens when building a POST_BUILD value for link targets that
  // don't use POST_BUILD.
  if (cmdLines.empty())
    return ":";

  // TODO: This will work only on Unix platforms. I don't
  // want to use a link.txt file because I will loose the benefit of the
  // $in variables. A discussion about dealing with multiple commands in
  // a rule is started here:
  // http://groups.google.com/group/ninja-build/browse_thread/thread/d515f23a78986008
  std::ostringstream cmd;
  for (std::vector<std::string>::const_iterator li = cmdLines.begin();
       li != cmdLines.end(); ++li) {
    if (li != cmdLines.begin())
      cmd << " && ";
    cmd << *li;
  }
  return cmd.str();
}

void cmLocalNinjaGenerator::AppendCustomCommandLines(const cmCustomCommand *cc, std::vector<std::string> &cmdLines) {
  cmCustomCommandGenerator ccg(*cc, this->GetConfigName(), this->Makefile);
  if (ccg.GetNumberOfCommands() > 0) {
    std::ostringstream cdCmd;
    cdCmd << "cd ";
    if (const char* wd = cc->GetWorkingDirectory())
      cdCmd << wd;
    else
      cdCmd << this->GetMakefile()->GetStartOutputDirectory();
    cmdLines.push_back(cdCmd.str());
  }
  for (unsigned i = 0; i != ccg.GetNumberOfCommands(); ++i) {
    cmdLines.push_back(ccg.GetCommand(i));
    std::string& cmd = cmdLines.back();
    ccg.AppendArguments(i, cmd);
  }
}

void
cmLocalNinjaGenerator::WriteCustomCommandBuildStatement(cmCustomCommand *cc,
                                             const cmNinjaDeps& orderOnlyDeps) {
  if (this->GetGlobalNinjaGenerator()->SeenCustomCommand(cc))
    return;

  const std::vector<std::string> &outputs = cc->GetOutputs();
  cmNinjaDeps ninjaOutputs(outputs.size()), ninjaDeps;

  std::transform(outputs.begin(), outputs.end(),
                 ninjaOutputs.begin(), MapToNinjaPath());
  this->AppendCustomCommandDeps(cc, ninjaDeps);

  for (cmNinjaDeps::iterator i = ninjaOutputs.begin(); i != ninjaOutputs.end();
       ++i)
    this->GetGlobalNinjaGenerator()->SeenCustomCommandOutput(*i);

  std::vector<std::string> cmdLines;
  this->AppendCustomCommandLines(cc, cmdLines);

  if (cmdLines.empty()) {
    cmGlobalNinjaGenerator::WritePhonyBuild(this->GetBuildFileStream(),
                                            "Phony custom command for " +
                                              ninjaOutputs[0],
                                            ninjaOutputs,
                                            ninjaDeps,
                                            cmNinjaDeps(),
                                            orderOnlyDeps,
                                            cmNinjaVars());
  } else {
    this->GetGlobalNinjaGenerator()->WriteCustomCommandBuild(
      this->BuildCommandLine(cmdLines),
      this->ConstructComment(*cc),
      "Custom command for " + ninjaOutputs[0],
      ninjaOutputs,
      ninjaDeps,
      orderOnlyDeps);
  }
}

void cmLocalNinjaGenerator::AddCustomCommandTarget(cmCustomCommand* cc, cmTarget* target)
{
  this->CustomCommandTargets[cc].insert(target);
}

void cmLocalNinjaGenerator::WriteCustomCommandBuildStatements()
{
  for (CustomCommandTargetMap::iterator i = this->CustomCommandTargets.begin();
       i != this->CustomCommandTargets.end(); ++i) {
    // A custom command may appear on multiple targets.  However, some build
    // systems exist where the target dependencies on some of the targets are
    // overspecified, leading to a dependency cycle.  If we assume all target
    // dependencies are a superset of the true target dependencies for this
    // custom command, we can take the set intersection of all target dependencies
    // to obtain a correct dependency list.
    std::set<cmTarget*>::iterator j = i->second.begin();
    assert(j != i->second.end());
    std::vector<std::string> ccTargetDeps;
    this->AppendTargetDepends(*j, ccTargetDeps);
    std::sort(ccTargetDeps.begin(), ccTargetDeps.end());
    ++j;

    for (; j != i->second.end(); ++j) {
      std::vector<std::string> jDeps, depsIntersection;
      this->AppendTargetDepends(*j, jDeps);
      std::sort(jDeps.begin(), jDeps.end());
      std::set_intersection(ccTargetDeps.begin(), ccTargetDeps.end(),
                            jDeps.begin(), jDeps.end(),
                            std::back_inserter(depsIntersection));
      ccTargetDeps = depsIntersection;
    }

    this->WriteCustomCommandBuildStatement(i->first, ccTargetDeps);
  }
}
