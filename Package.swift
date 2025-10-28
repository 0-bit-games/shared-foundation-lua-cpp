// swift-tools-version:5.9

//
// Package.swift
// foundation-lua
//
// Created by Kristian Trenskow on 2023/10/07
// See license in LICENSE.
//

import PackageDescription

let package = Package(
	name: "FoundationLuaCxx",
	products: [
		.library(
			name: "FoundationLuaCxx",
			targets: ["FoundationLuaCxx"])
	],
	dependencies: [
		.package(name: "SharedFoundationCxx", path: "../shared-foundation-cpp")
	],
	targets: [
		.target(
			name: "FoundationLuaCxx",
			dependencies: [
				.product(name: "FoundationCxx", package: "SharedFoundationCxx")
			],
			path: ".",
			exclude: [
				"README.md",
				"LICENSE",
				"src/foundation",
				"./src/lua/lua-5.4.8/src/luac.c",
				"./src/lua/lua-5.4.8/src/lua.c",
				"./src/lua/lua-5.4.8-tests",
				"./src/lua/lua-config-package-tests"
			],
			publicHeadersPath: "./include",
			cSettings: [
				.headerSearchPath("./src/lua/lua-5.4.8/include"),
				.define("LUA_USE_IOS")
			],
			cxxSettings: [
				.headerSearchPath("./src/lua/lua-5.4.8/include"),
				.define("LUA_USE_IOS")
			],
			swiftSettings: [
				.interoperabilityMode(.Cxx)
			]
		)
	],
	cxxLanguageStandard: .cxx20
)
