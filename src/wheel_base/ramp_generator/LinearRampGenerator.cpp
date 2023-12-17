#include "LinearRampGenerator.h"

LinearRampGenerator::LinearRampGenerator();

void LinearRampGenerator::updateCurve(float acceleration, int startSpeed, int finalSpeed)
{
    m_acceleration = acceleration;
    m_startSpeed = startSpeed;
    m_finalSpeed = finalSpeed;
}

uint32_t LinearRampGenerator::getFullAccelerationDistance() const
{
    auto distance = (m_finalSpeed - m_startSpeed) / m_acceleration;
    if (distance < 0)
    {
        // Instead of std::abs
        distance = -1 * distance;
    }

    return static_cast<uint32_t>(distance);
}

int LinearRampGenerator::getSpeedForDistance(uint32_t passedDistance)
{
    // y = k * x + b = dy/dx * x + b
    // dy/dx = acceleration
    int speed = m_acceleration * passedDistance + m_startSpeed;
    return (speed > m_finalSpeed) ? m_finalSpeed : speed;
}
