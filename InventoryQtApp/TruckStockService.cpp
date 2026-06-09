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
            truck.updatedAt = item.value("updatedAt", item.value("createdAt", ""));

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

bool TruckStockService::updateTruck(
    const std::string& truckId,
    const UpdateTruckRequest& request
)
{
    try {
        json body;

        if (!request.truckNumber.empty()) {
            body["truckNumber"] = request.truckNumber;
        }

        if (!request.plateNumber.empty()) {
            body["plateNumber"] = request.plateNumber;
        }

        body["technicianId"] = request.technicianId;

        if (!request.status.empty()) {
            body["status"] = request.status;
        }

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

bool TruckStockService::deactivateTruck(
    const std::string& truckId
)
{
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
            assignment.status = item.value("status", "Active");

            if (item.contains("truck") && item["truck"].is_object()) {
                assignment.truckId =
                    item["truck"].value("id", "");

                assignment.truckNumber =
                    item["truck"].value("truckNumber", "");
            }

            if (item.contains("template") && item["template"].is_object()) {
                assignment.templateId =
                    item["template"].value("id", "");

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

TemplateDetailsDto TruckStockService::getTemplateById(
    const std::string& templateId
)
{
    TemplateDetailsDto templateDetails;

    try {
        auto response = apiClient.get(
            "/truck-stock/templates/" + templateId
        );

        if (response.status_code != 200) {
            std::cerr << "GET /truck-stock/templates/:id failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return templateDetails;
        }

        json data = json::parse(response.text);

        templateDetails.id = data.value("id", "");
        templateDetails.name = data.value("name", "");
        templateDetails.tradeType = data.value("tradeType", "");

        if (data.contains("items") && data["items"].is_array()) {
            for (const auto& itemJson : data["items"]) {
                CreateTemplateItemRequest item;

                item.productName =
                    itemJson.value("productName", "");

                item.category =
                    itemJson.value("category", "");

                item.requiredQuantity =
                    itemJson.value("requiredQuantity", 1);

                item.minimumQuantity =
                    itemJson.value("minimumQuantity", 1);

                item.expectedPrice =
                    itemJson.value("expectedPrice", 0.0);

                item.unit =
                    itemJson.value("unit", "");

                item.notes =
                    itemJson.value("notes", "");

                templateDetails.items.push_back(item);
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::getTemplateById error: "
            << ex.what()
            << std::endl;
    }

    return templateDetails;
}

bool TruckStockService::updateTemplate(
    const std::string& templateId,
    const CreateTemplateRequest& request
)
{
    try {
        json body;

        body["name"] = request.name;
        body["tradeType"] = request.tradeType;

        if (!request.items.empty()) {
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
        }

        auto response = apiClient.put(
            "/truck-stock/templates/" + templateId,
            body.dump()
        );

        if (response.status_code != 200) {
            std::cerr << "PUT /truck-stock/templates/:id failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::updateTemplate error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::deleteTemplate(
    const std::string& templateId
)
{
    try {
        auto response = apiClient.del(
            "/truck-stock/templates/" + templateId
        );

        if (response.status_code != 200 && response.status_code != 204) {
            std::cerr << "DELETE /truck-stock/templates/:id failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::deleteTemplate error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

MyTruckStockDto TruckStockService::getMyTruckStock()
{
    MyTruckStockDto result;

    try {
        auto response = apiClient.get("/truck-stock/my-stock");

        if (response.status_code != 200) {
            std::cerr << "GET /truck-stock/my-stock failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return result;
        }

        json data = json::parse(response.text);

        result.truckId =
            data.value("id", "");

        result.truckNumber =
            data.value("truckNumber", "");

        result.plateNumber =
            data.value("plateNumber", "");

        if (data.contains("stockAssignments") &&
            data["stockAssignments"].is_array()) {

            for (const auto& assignment : data["stockAssignments"]) {

                if (!assignment.contains("template")) {
                    continue;
                }

                const auto& templateData = assignment["template"];

                if (!templateData.contains("items") ||
                    !templateData["items"].is_array()) {
                    continue;
                }

                for (const auto& itemJson : templateData["items"]) {
                    MyTruckStockItemDto item;

                    item.id =
                        itemJson.value("id", "");

                    item.productName =
                        itemJson.value("productName", "");

                    item.category =
                        itemJson.value("category", "");

                    item.currentQuantity =
                        itemJson.value("currentQuantity", 0);

                    item.minimumQuantity =
                        itemJson.value("minimumQuantity", 0);

                    item.requiredQuantity =
                        itemJson.value("requiredQuantity", 0);

                    item.status =
                        item.currentQuantity <= item.minimumQuantity
                        ? "Low Stock"
                        : "OK";

                    result.items.push_back(item);
                }
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::getMyTruckStock error: "
            << ex.what()
            << std::endl;
    }

    return result;
}

bool TruckStockService::useTruckItem(
    const UseTruckItemRequest& request
)
{
    try {
        json body;

        body["truckStockItemId"] = request.itemId;
        body["quantity"] = request.quantityUsed;
        body["notes"] = request.notes;

        auto response = apiClient.post(
            "/truck-stock/use-item",
            body.dump()
        );

        if (response.status_code != 200 && response.status_code != 201) {
            std::cerr << "POST /truck-stock/use-item failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::useTruckItem error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

std::vector<LowStockItemDto> TruckStockService::getLowStockItems()
{
    std::vector<LowStockItemDto> lowStockItems;

    try {
        auto response = apiClient.get("/truck-stock/low-stock");

        if (response.status_code != 200) {
            std::cerr << "GET /truck-stock/low-stock failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return lowStockItems;
        }

        json data = json::parse(response.text);

        for (const auto& templateJson : data) {
            std::string templateId =
                templateJson.value("id", "");

            std::string templateName =
                templateJson.value("name", "");

            std::string truckId = "";
            std::string truckNumber = "";

            if (templateJson.contains("assignments") &&
                templateJson["assignments"].is_array() &&
                !templateJson["assignments"].empty()) {

                const auto& firstAssignment =
                    templateJson["assignments"][0];

                if (firstAssignment.contains("truck") &&
                    firstAssignment["truck"].is_object()) {

                    truckId =
                        firstAssignment["truck"].value("id", "");

                    truckNumber =
                        firstAssignment["truck"].value("truckNumber", "");
                }
            }

            if (!templateJson.contains("items") ||
                !templateJson["items"].is_array()) {
                continue;
            }

            for (const auto& itemJson : templateJson["items"]) {
                LowStockItemDto item;

                item.itemId =
                    itemJson.value("id", "");

                item.templateId =
                    templateId;

                item.templateName =
                    templateName;

                item.truckId =
                    truckId;

                item.truckNumber =
                    truckNumber;

                item.productName =
                    itemJson.value("productName", "");

                item.currentQuantity =
                    itemJson.value("currentQuantity", 0);

                item.minimumQuantity =
                    itemJson.value("minimumQuantity", 0);

                item.status =
                    item.currentQuantity <= 0
                    ? "Critical"
                    : "Warning";

                lowStockItems.push_back(item);
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::getLowStockItems error: "
            << ex.what()
            << std::endl;
    }

    return lowStockItems;
}

std::vector<ReceiptDto> TruckStockService::getReceipts()
{
    std::vector<ReceiptDto> receipts;

    try {
        auto response = apiClient.get("/truck-stock/receipts");

        if (response.status_code != 200) {
            std::cerr << "GET /truck-stock/receipts failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return receipts;
        }

        json data = json::parse(response.text);

        for (const auto& item : data) {
            ReceiptDto receipt;

            receipt.id = item.value("id", "");
            receipt.totalAmount = item.value("totalAmount", 0.0);
            receipt.status = item.value("status", "");
            receipt.createdAt = item.value("createdAt", "");
            receipt.fileUrl = item.value("fileUrl", "");

            if (item.contains("technician") && item["technician"].is_object()) {
                receipt.technicianName =
                    item["technician"].value("name", "");

                if (receipt.technicianName.empty()) {
                    receipt.technicianName =
                        item["technician"].value("email", "");
                }
            }

            if (item.contains("truck") && item["truck"].is_object()) {
                receipt.truckNumber =
                    item["truck"].value("truckNumber", "");
            }

            receipts.push_back(receipt);
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::getReceipts error: "
            << ex.what()
            << std::endl;
    }

    return receipts;
}

std::vector<TruckStockMovementDto> TruckStockService::getMovements()
{
    std::vector<TruckStockMovementDto> movements;

    try {
        auto response =
            apiClient.get("/truck-stock/movements");

        if (response.status_code != 200) {
            std::cerr
                << "GET /truck-stock/movements failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return movements;
        }

        json data =
            json::parse(response.text);

        for (const auto& item : data) {
            TruckStockMovementDto movement;

            movement.id =
                item.value("id", "");

            movement.action =
                item.value("action", "");

            movement.previousQuantity =
                item.value("previousQuantity", 0);

            movement.newQuantity =
                item.value("newQuantity", 0);

            movement.notes =
                item.value("notes", "");

            movement.createdAt =
                item.value("createdAt", "");

            if (
                item.contains("truckStockItem") &&
                item["truckStockItem"].is_object()
                ) {
                movement.truckStockItemId =
                    item["truckStockItem"].value("id", "");

                movement.productName =
                    item["truckStockItem"].value("productName", "");
            }

            if (
                item.contains("changedBy") &&
                item["changedBy"].is_object()
                ) {
                movement.changedBy =
                    item["changedBy"].value("email", "");
            }

            movements.push_back(movement);
        }
    }
    catch (const std::exception& ex) {
        std::cerr
            << "TruckStockService::getMovements error: "
            << ex.what()
            << std::endl;
    }

    return movements;
}

bool TruckStockService::createReceipt(
    const CreateReceiptRequest& request
)
{
    try {
        json body;

        body["truckId"] = request.truckId;
        body["fileUrl"] = request.fileUrl;
        body["totalAmount"] = request.totalAmount;

        auto response = apiClient.post(
            "/truck-stock/receipts",
            body.dump()
        );

        if (response.status_code != 200 && response.status_code != 201) {
            std::cerr << "POST /truck-stock/receipts failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::createReceipt error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::approveReceipt(
    const std::string& receiptId
)
{
    try {
        json body;

        body["status"] = "APPROVED";

        auto response = apiClient.patch(
            "/truck-stock/receipts/" + receiptId + "/status",
            body.dump()
        );

        if (response.status_code != 200) {
            std::cerr << "PATCH approve receipt failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::approveReceipt error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::rejectReceipt(
    const std::string& receiptId
)
{
    try {
        json body;

        body["status"] = "REJECTED";

        auto response = apiClient.patch(
            "/truck-stock/receipts/" + receiptId + "/status",
            body.dump()
        );

        if (response.status_code != 200) {
            std::cerr << "PATCH reject receipt failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "TruckStockService::rejectReceipt error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::unassignTemplate(
    const std::string& assignmentId
)
{
    try {
        auto response = apiClient.del(
            "/truck-stock/assignments/" + assignmentId
        );

        if (
            response.status_code != 200 &&
            response.status_code != 204
            ) {
            std::cerr
                << "DELETE assignment failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr
            << "TruckStockService::unassignTemplate error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::updateAssignment(
    const std::string& assignmentId,
    const UpdateAssignmentRequest& request
)
{
    try {
        json body;

        body["truckId"] = request.truckId;
        body["templateId"] = request.templateId;

        if (!request.status.empty()) {
            body["status"] = request.status;
        }

        auto response =
            apiClient.put(
                "/truck-stock/assignments/" + assignmentId,
                body.dump()
            );

        if (response.status_code != 200) {
            std::cerr
                << "PUT assignment failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr
            << "TruckStockService::updateAssignment error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool TruckStockService::restockTruckStockItem(
    const std::string& itemId,
    int newQuantity
)
{
    try {
        json body;

        body["quantity"] = newQuantity;

        auto response =
            apiClient.patch(
                "/truck-stock/items/" + itemId + "/quantity",
                body.dump()
            );

        if (response.status_code != 200) {
            std::cerr
                << "PATCH restock item failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr
            << "TruckStockService::restockTruckStockItem error: "
            << ex.what()
            << std::endl;

        return false;
    }
}