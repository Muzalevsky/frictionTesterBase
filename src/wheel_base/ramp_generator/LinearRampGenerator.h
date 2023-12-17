#pragma once

class LinearRampGenerator
{
public:
    LinearRampGenerator();
    void getSpeedForDistance(uint32_t passedDistance) const;
    void updateCurve(float acceleration, int startSpeed, int finalSpeed);
private:
    float m_acceleration{0};
    int m_startSpeed{0};
    int m_finalSpeed{0};
};
