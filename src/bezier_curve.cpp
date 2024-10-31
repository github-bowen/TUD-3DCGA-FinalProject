// bezier_curve.cpp
#include "bezier_curve.h"

BezierCurve::BezierCurve() = default;

// add one segment of cubic Bezier Curve 
void BezierCurve::addSegment(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    segments.push_back({ p0, p1, p2, p3 });
}

// set control points of specified segment
void BezierCurve::setSegmentControlPoints(size_t segmentIndex, const glm::vec3& p0, 
    const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    if (segmentIndex < segments.size()) {
        segments[segmentIndex] = { p0, p1, p2, p3 };
    } else {
		throw std::out_of_range("Error in BezierCurve::setSegmentControlPoints: \nSegment index out of range! Given index: "
			+ std::to_string(segmentIndex) + ", Segment count: " + std::to_string(segments.size()));
    }
}

// get point on path at given time (time belongs to [0, 1])
glm::vec3 BezierCurve::getPointOnPath(float time) const {
	if (segments.empty()) return glm::vec3(0.0f);  // return 0 vector if no segments

	// calculate current segment index at given time
	float segmentDuration = 1.0f / segments.size();  // duration of each segment
    int segmentIndex = std::min(static_cast<int>(time * segments.size()), static_cast<int>(segments.size() - 1));
	float t = (time - segmentIndex * segmentDuration) / segmentDuration; // local t value for current segment

	// get control points of current segment and calculate point
    const std::vector<glm::vec3>& controlPoints = segments[segmentIndex];
    return calculateCubicBezierPoint(t, controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);
}

// return total segment count
size_t BezierCurve::getSegmentCount() const {
    return segments.size();
}

// get control points of specified segment
std::vector<glm::vec3> BezierCurve::getSegmentControlPoints(size_t segmentIndex) const {
    if (segmentIndex < segments.size()) {
        return segments[segmentIndex];
    } else {
        throw std::out_of_range("Error in BezierCurve::getSegmentControlPoints: \nSegment index out of range! Given index: "
            + std::to_string(segmentIndex) + ", Segment count: " + std::to_string(segments.size()));
    }
}

// calculate point of cubic Bezier curve
glm::vec3 BezierCurve::calculateCubicBezierPoint(float t, const glm::vec3& p0, const glm::vec3& p1, 
    const glm::vec3& p2, const glm::vec3& p3) const {

    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * p0;        // (1 - t)^3 * p0
    point += 3 * uu * t * p1;          // 3 * (1 - t)^2 * t * p1
    point += 3 * u * tt * p2;          // 3 * (1 - t) * t^2 * p2
    point += ttt * p3;                 // t^3 * p3

    return point;
}
