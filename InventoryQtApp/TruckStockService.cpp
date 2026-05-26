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

std::vector<StockTemplateDto> TruckStockService::getTemplates()
{
    std::vector<StockTemplateDto> templates;

    try {
        auto response = apiClient.get("/truck-stock/templates");

        if (response.status_code != 200) {
            std::cerr << "GET /truck-stock/templates failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return templates;
        }

        json data = json::parse(response.text);

        for (const auto& item : data) {
            StockTemplateDto stockTemplate;

            stockTemplate.id = item.value("id", "");
            stockTemplate.name = item.value("name", "");
            stockTemplate.tradeType = item.value("tradeType", "");

            if (item.contains("items") && item["items"].is_array()) {
                stockTemplate.itemCount =
                    static_cast<int>(item["items"].size());
            }
            else {
                stockTemplate.itemCount = 0;
            }

            templates.push_back(stockTemplate);
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::getTemplates error: "
            << ex.what()
            << std::endl;
    }

    return templates;
}

bool TruckStockService::createTemplate(
    const CreateTemplateRequest& request
)
{
    try {
        json body;

        body["name"] = request.name;
        body["tradeType"] = request.tradeType;
        body["items"] = json::array();

        for (const auto& item : request.items) {
            json itemJson;

            itemJson["productName"] = item.productName;
            itemJson["category"] = item.category;
            itemJson["requiredQuantity"] = item.requiredQuantity;
            itemJson["minimumQuantity"] = item.minimumQuantity;
            itemJson["expectedPrice"] = item.expectedPrice;
            itemJson["unit"] = item.unit;
            itemJson["notes"] = item.notes;

            body["items"].push_back(itemJson);
        }

        auto response = apiClient.post(
            "/truck-stock/templates",
            body.dump()
        );

        if (response.status_code != 200 && response.status_code != 201) {
            std::cerr << "POST /truck-stock/templates failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::createTemplate error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::assignTemplate(
    const CreateAssignmentRequest& request
)
{
    try {
        json body;

        body["truckId"] = request.truckId;
        body["templateId"] = request.templateId;

        auto response = apiClient.post(
            "/truck-stock/assignments",
            body.dump()
        );

        if (response.status_code != 200 && response.status_code != 201) {
            std::cerr << "POST /truck-stock/assignments failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::assignTemplate error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

std::vector<TruckAssignmentDto> TruckStockService::getAssignments()
{
    std::vector<TruckAssignmentDto> assignments;

    try {
        auto response = apiClient.get("/truck-stock/assignments");

        if (response.status_code != 200) {
            std::cerr << "GET /truck-stock/assignments failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return assignments;
        }

        json data = json::parse(response.text);

        for (const auto& item : data) {
            TruckAssignmentDto assignment;

            assignment.id = item.value("id", "");
            assignment.assignedOn = item.value("createdAt", "");
            assignment.status = "Active";

            if (item.contains("truck") && item["truck"].is_object()) {
                assignment.truckNumber =
                    item["truck"].value("truckNumber", "");
            }

            if (item.contains("template") && item["template"].is_object()) {
                assignment.templateName =
                    item["template"].value("name", "");
            }

            if (item.contains("assignedBy") && item["assignedBy"].is_object()) {
                assignment.assignedBy =
                    item["assignedBy"].value("name", "");

                if (assignment.assignedBy.empty()) {
                    assignment.assignedBy =
                        item["assignedBy"].value("email", "");
                }
            }

            assignments.push_back(assignment);
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::getAssignments error: "
            << ex.what()
            << std::endl;
    }

    return assignments;
}