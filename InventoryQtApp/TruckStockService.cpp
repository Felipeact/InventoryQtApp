#include "TruckStockService.h"

#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

TruckStockService::TruckStockService(ApiClient& apiClient)
    : apiClient(apiClient)
{
}

std::vector<TruckDto> TruckStockService::getTrucks()
{
    std::vector<TruckDto> trucks;

    try {
        auto response = apiClient.get("/truck-stock/trucks");

        if (response.status_code != 200) {
            std::cerr << "GET /truck-stock/trucks failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return trucks;
        }

        json data = json::parse(response.text);

        for (const auto& item : data) {
            TruckDto truck;

            truck.id = item.value("id", "");
            truck.truckName = item.value("truckNumber", "");
            truck.licensePlate = item.value("plateNumber", "");
            truck.status = item.value("status", "");

            if (item.contains("technician") && item["technician"].is_object()) {
                truck.technicianId = item["technician"].value("id", "");
                truck.technicianName = item["technician"].value("name", "");

                if (truck.technicianName.empty()) {
                    truck.technicianName = item["technician"].value("email", "");
                }
            }
            else {
                truck.technicianId = "";
                truck.technicianName = "";
            }

            trucks.push_back(truck);
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::getTrucks error: "
            << ex.what()
            << std::endl;
    }

    return trucks;
}

bool TruckStockService::createTruck(const CreateTruckRequest& request)
{
    try {
        json body;

        body["truckNumber"] = request.truckNumber;
        body["plateNumber"] = request.plateNumber;
        body["technicianId"] = request.technicianId;
        body["status"] = request.status;

        auto response = apiClient.post(
            "/truck-stock/trucks",
            body.dump()
        );

        if (response.status_code != 200 && response.status_code != 201) {
            std::cerr << "POST /truck-stock/trucks failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::createTruck error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::updateTruck(const std::string& truckId, const UpdateTruckRequest& request)
{
    try {
        json body;

        body["truckNumber"] = request.truckNumber;
        body["plateNumber"] = request.plateNumber;
        body["technicianId"] = request.technicianId;
        body["status"] = request.status;

        auto response = apiClient.put(
            "/truck-stock/trucks/" + truckId,
            body.dump()
        );

        if (response.status_code != 200) {
            std::cerr << "PUT /truck-stock/trucks failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::updateTruck error: "
            << ex.what()
            << std::endl;

        return false;
    }

}

bool TruckStockService::deactivateTruck(const std::string& truckId)
{
    UpdateTruckRequest request;

    request.truckNumber = "";
    request.plateNumber = "";
    request.technicianId = "";
    request.status = "INACTIVE";

    try {
        json body;
        body["status"] = "INACTIVE";
        body["technicianId"] = "";

        auto response = apiClient.put(
            "/truck-stock/trucks/" + truckId,
            body.dump()
        );

        return response.status_code == 200;
    }
    catch (...) {
        return false;
    }
}