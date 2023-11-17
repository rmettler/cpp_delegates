#include <iostream>
#include <rome/delegate.hpp>

struct Engine {
    void accelerate() {
        std::cout << "engine accelerating\n";
    }
    void turnOff() {
        std::cout << "engine turned off\n";
    }
};

struct BrakingSystem {
    void turnBrakesOn() {
        std::cout << "brakes on\n";
    }
    void turnBrakesOff() {
        std::cout << "brakes off\n";
    }
};

struct SpeedSensor {
    // Assigning delegate is optional for speed sensor to work.
    rome::event_delegate<void(float)> onSpeedChanged;
};

class CruiseControl {
    float targetSpeed_ = 0.0F;

    // Assigning both delegates is required for cruise control to work.
    rome::command_delegate<void()> onAccelerateCar_;
    rome::command_delegate<void()> onSlowDownCar_;

  public:
    void updateAcceleration(const float drivingSpeed) {
        if (drivingSpeed < targetSpeed_ * 0.95F) {
            onAccelerateCar_();
        }
        else if (drivingSpeed > targetSpeed_ * 1.05F) {
            onSlowDownCar_();
        }
    }

    void setTargetSpeed(const float targetSpeed) {
        targetSpeed_ = targetSpeed;
    }

    CruiseControl(rome::command_delegate<void()>&& onAccelerateCar,
        rome::command_delegate<void()>&& onSlowDownCar)
        : onAccelerateCar_{std::move(onAccelerateCar)}, onSlowDownCar_{std::move(onSlowDownCar)} {
    }
};

struct Integration {
    SpeedSensor speedSensor;
    CruiseControl cruiseControl;
    Engine engine;
    BrakingSystem brakes;

    Integration()
        : cruiseControl{
            [this]() {
                brakes.turnBrakesOff();
                engine.accelerate();
            },
            [this]() {
                engine.turnOff();
                brakes.turnBrakesOn();
            }} {
        speedSensor.onSpeedChanged = [this](float drivingSpeed) {
            cruiseControl.updateAcceleration(drivingSpeed);
        };
    }
};

Integration integration{};

int main() {
    // Simulate IO not connected in this example
    integration.cruiseControl.setTargetSpeed(25.0F);
    integration.speedSensor.onSpeedChanged(20.0F);
    integration.speedSensor.onSpeedChanged(25.0F);
    integration.speedSensor.onSpeedChanged(30.0F);
}
