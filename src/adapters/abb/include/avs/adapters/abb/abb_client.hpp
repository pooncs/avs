#pragma once
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "avs/core/result.hpp"

namespace avs::adapters::abb {

struct JointPositions { std::vector<double> q; };
struct Pose { double x{}, y{}, z{}, qx{}, qy{}, qz{}, qw{}; };

class IAbbClient {
public:
    virtual ~IAbbClient() = default;
    virtual avs::core::Result connect(const std::string& ip, int port = 80) = 0;
    virtual bool is_connected() const = 0;
    virtual void disconnect() = 0;

    // RAPID variables (basic types)
    virtual avs::core::Result read_rapid(const std::string& task, const std::string& name, std::string& out) = 0;
    virtual avs::core::Result read_rapid(const std::string& task, const std::string& name, double& out) = 0;
    virtual avs::core::Result read_rapid(const std::string& task, const std::string& name, bool& out) = 0;

    virtual avs::core::Result write_rapid(const std::string& task, const std::string& name, const std::string& value) = 0;
    virtual avs::core::Result write_rapid(const std::string& task, const std::string& name, double value) = 0;
    virtual avs::core::Result write_rapid(const std::string& task, const std::string& name, bool value) = 0;

    // Motions
    virtual avs::core::Result moveJ(const JointPositions& q, double speed = 50.0, double zone = 5.0) = 0;
    virtual avs::core::Result moveJ(const Pose& p, double speed = 50.0, double zone = 5.0) = 0;
    virtual avs::core::Result moveL(const Pose& p, double speed = 50.0, double zone = 1.0) = 0;
};

class AbbClient : public IAbbClient {
public:
    AbbClient();
    ~AbbClient() override;

    avs::core::Result connect(const std::string& ip, int port = 80) override;
    bool is_connected() const override;
    void disconnect() override;

    avs::core::Result read_rapid(const std::string& task, const std::string& name, std::string& out) override;
    avs::core::Result read_rapid(const std::string& task, const std::string& name, double& out) override;
    avs::core::Result read_rapid(const std::string& task, const std::string& name, bool& out) override;

    avs::core::Result write_rapid(const std::string& task, const std::string& name, const std::string& value) override;
    avs::core::Result write_rapid(const std::string& task, const std::string& name, double value) override;
    avs::core::Result write_rapid(const std::string& task, const std::string& name, bool value) override;

    avs::core::Result moveJ(const JointPositions& q, double speed = 50.0, double zone = 5.0) override;
    avs::core::Result moveJ(const Pose& p, double speed = 50.0, double zone = 5.0) override;
    avs::core::Result moveL(const Pose& p, double speed = 50.0, double zone = 1.0) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
