#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <ShlObj.h>

#include "../rbx_engine.h"
#include "C:\Users\Caoimhin\Desktop\Main src\idk what to name yet lol\src\driver/driver_impl.hpp"
#include "../../misc/output/output.hpp"
#include "../../misc/globals/globals.hpp"

#include <iostream>
#include <vector>
#include <string>

uintptr_t RBX::TaskScheduler::GetScheduler() {
     //   utils::output::info("Fetching Scheduler. Virtual Address: " + std::to_string(virtualaddy));
    uintptr_t result = virtualaddy ? driver::read<uintptr_t>(virtualaddy + 0x5D334E8) : 0;
    //   utils::output::info("Scheduler Address: " + std::to_string(result));
    return result;
}

std::vector<RBX::Instance> RBX::TaskScheduler::GetJobs() {
    std::vector<RBX::Instance> jobs;
    
    if (!GetScheduler()) {
     //   utils::output::warning("No scheduler found. Returning empty job list.");
        return jobs;
    }

    uintptr_t jobStart = driver::read<uintptr_t>(GetScheduler() + 0x1C8);
    uintptr_t jobEnd = driver::read<uintptr_t>(GetScheduler() + 0x1D0);
     // utils::output::info("Job Start: " + std::to_string(jobStart) + " Job End: " + std::to_string(jobEnd));

    if (jobStart && jobEnd && jobStart < jobEnd) {
        for (uintptr_t job = jobStart; job < jobEnd; job += 0x10) {
            uintptr_t jobAddress = driver::read<uintptr_t>(job);
              //     utils::output::info("Job Address: " + std::to_string(jobAddress));
            if (jobAddress) jobs.emplace_back(jobAddress);
        }
    }
    return jobs;
}

std::string RBX::TaskScheduler::GetJobName(RBX::Instance instance) {
    std::string jobName = driver::ReadString(instance.address + 0x90);
    //   utils::output::info("Job Address: " + std::to_string(instance.address) + " Name: " + jobName);
    return jobName;
}

uint64_t RBX::TaskScheduler::GetJobByName(const std::string& targetName) {
  //    utils::output::info("Searching for Job: " + targetName);
    for (const auto& job : GetJobs()) {
        std::string jobName = GetJobName(job);
        if (jobName == targetName) {
         //           utils::output::info("Found Job " + targetName + " at address: " + std::to_string(job.address));
            return job.address;
        }
    }
   //  utils::output::warning("Job " + targetName + " not found.");
    return 0;
}

uint64_t RBX::TaskScheduler::GetRenderJob() {
      // utils::output::info("Fetching RenderJob");
    return GetJobByName("RenderJob");
}

unsigned int RBX::TaskScheduler::GetTargetFPS() {
    double rawFps = driver::read<double>(GetScheduler() + 0x178);
    return static_cast<unsigned int>(1.0 / rawFps);
}

void RBX::TaskScheduler::SetTargetFPS(unsigned int targetFps) {
    if (targetFps > 0) {
        double newFps = 1.0 / static_cast<double>(targetFps);
        driver::write<double>(GetScheduler() + 0x178, newFps);
    }
}

void RBX::TaskScheduler::PauseTask(uintptr_t jobAddress) {
    if (jobAddress) {
        driver::write<bool>(jobAddress + 0x10, true);
    //    utils::output::print("Task at " + std::to_string(jobAddress) + " paused.");
    }
}

void RBX::TaskScheduler::ResumeTask(uintptr_t jobAddress) {
    if (jobAddress) {
        driver::write<bool>(jobAddress + 0x10, false);
      //  utils::output::print("Task at " + std::to_string(jobAddress) + " resumed.");
    }
}

bool RBX::TaskScheduler::RemoveTaskByName(const std::string& targetName) {
    uintptr_t jobListStart = driver::read<uintptr_t>(GetScheduler() + 0x1C8);
    for (uintptr_t currentJob = jobListStart; ; currentJob += 0x10) {
        uintptr_t jobAddress = driver::read<uintptr_t>(currentJob);
        if (jobAddress == 0) break;

        std::string jobName = driver::ReadString(jobAddress + 0x90);
        if (jobName == targetName) {
            driver::write<uintptr_t>(currentJob, 0);
       //     utils::output::print("Removed job with name: " + jobName);
            return true;
        }
    }
    return false;
}

unsigned int RBX::TaskScheduler::GetJobCount() {
    unsigned int jobCount = 0;
    uintptr_t jobListStart = driver::read<uintptr_t>(GetScheduler() + 0x1C8);
    for (uintptr_t currentJob = jobListStart; ; currentJob += 0x10) {
        uintptr_t jobAddress = driver::read<uintptr_t>(currentJob);
        if (jobAddress == 0) break;
        ++jobCount;
    }
    return jobCount;
}

void RBX::TaskScheduler::GetJobDetails(uintptr_t jobAddress) {
    if (jobAddress) {
        std::string jobName = driver::ReadString(jobAddress + 0x90);
        unsigned int jobPriority = driver::read<unsigned int>(jobAddress + 0xC);
      //  utils::output::print("Job Name: " + jobName + ", Job Priority: " + std::to_string(jobPriority));
    }
}

void RBX::TaskScheduler::QueueTask(uintptr_t taskAddress) {
    uintptr_t schedulerAddress = GetScheduler();
    uintptr_t jobListStart = driver::read<uintptr_t>(schedulerAddress + 0x1C8);
    driver::write<uintptr_t>(jobListStart, taskAddress);
  //  utils::output::print("Task queued at address: " + std::to_string(taskAddress));
}

void RBX::TaskScheduler::ClearScheduler() {
    uintptr_t schedulerAddress = GetScheduler();
    uintptr_t jobListStart = driver::read<uintptr_t>(schedulerAddress + 0x1C8);
    driver::write<uintptr_t>(jobListStart, 0);
  //  utils::output::print("Scheduler cleared.");
}

bool RBX::TaskScheduler::IsTaskRunning(uintptr_t jobAddress) {
    return driver::read<bool>(jobAddress + 0x14);
}

void RBX::TaskScheduler::UpdateJobPriority(uintptr_t jobAddress, unsigned int newPriority) {
    if (jobAddress) {
        driver::write<unsigned int>(jobAddress + 0xC, newPriority);
      //  utils::output::print("Updated priority of job at " + std::to_string(jobAddress) + " to " + std::to_string(newPriority));
    }
}
