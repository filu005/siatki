#pragma once

// screen space constants
namespace c
{
	auto const screenWidth = 800u;
	auto const screenHeight = 600u;

	const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	const float viewHeight = 1.2f;
	const float viewWidth = aspectRatio * viewHeight;// 1.6
}

// grid constants
namespace c
{
	// tylko potegi 2 (bo powstaja niedokladnosci przy dzieleniu m.in. przy dx/dy/dz)
	auto const K = 32u;
	auto const L = 32u;
	auto const KL = K * L;

	auto const ymin = -1.0f, xmin = -1.0f;
	auto const ymax = 1.0f, xmax = 1.0f;
	const float dx = (xmax - xmin) / static_cast<float>(K);
	const float dy = (ymax - ymin) / static_cast<float>(L);
}