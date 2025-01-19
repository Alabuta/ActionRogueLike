// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRogueLikeTarget : TargetRules
{
	public ActionRogueLikeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bLegacyParentIncludePaths = false;
		CppStandard = CppStandardVersion.Default;
		WindowsPlatform.bStrictConformanceMode = true;
		bValidateFormatStrings = true;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ActionRogueLike");
	}
}
