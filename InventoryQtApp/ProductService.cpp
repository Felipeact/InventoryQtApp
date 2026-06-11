#include "ProductService.h"

#include <iostream>
#include <QtGlobal>
#include <chrono>

static std::string cacheBuster()
{
    auto now =
        std::chrono::system_clock::now()
        .time_since_epoch();

    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now)
        .count();

    return std::to_string(ms);
}

ProductService::ProductService(ApiClient& apiClient)
    : api(apiClient)
{
}

bool ProductService::createProduct(
    const std::string& name,
    const std::string& barcode,
    int quantity,
    const std::string& location,
    const std::string& description,
    int lowStockThreshold
)
{
    json body = {
        {"name", name},
        {"barcode", barcode},
        {"quantity", quantity},
        {"location", location},
        {"description", description},
        {"lowStockThreshold", lowStockThreshold}
    };

    auto res = api.post("/products", body.dump());

    if (res.status_code != 200 && res.status_code != 201) {
        std::cout << "Create product failed: " << res.text << std::endl;
        return false;
    }

    return true;
}

json ProductService::getProducts(bool forceRefresh)
{
    Q_UNUSED(forceRefresh);

    auto res = api.get("/products?page=1&limit=100&_=" + cacheBuster());

    if (res.status_code != 200) {
        std::cout << "Get products failed: " << res.text << std::endl;
        return json::array();
    }

    try {
        auto response = json::parse(res.text);

        if (response.contains("data") && response["data"].is_array()) {
            return response["data"];
        }

        if (response.is_array()) {
            return response;
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Parse products failed: " << ex.what() << std::endl;
    }

    return json::array();
}

bool ProductService::updateProduct(
    const std::string& productId,
    const std::string& name,
    const std::string& barcode,
    int quantity,
    const std::string& location,
    const std::string& description,
    int lowStockThreshold
)
{
    json body = {
        {"name", name},
        {"barcode", barcode},
        {"quantity", quantity},
        {"location", location},
        {"description", description},
        {"lowStockThreshold", lowStockThreshold}
    };

    auto res = api.put("/products/" + productId, body.dump());

    if (res.status_code != 200) {
        std::cout << "Update product failed: " << res.text << std::endl;
        return false;
    }

    return true;
}

bool ProductService::deleteProduct(const std::string& productId)
{
    auto res = api.del("/products/" + productId);

    if (res.status_code != 200) {
        std::cout << "Delete product failed: " << res.text << std::endl;
        return false;
    }

    return true;
}

json ProductService::searchProducts(const std::string& searchText)
{
    std::string endpoint = "/products?page=1&limit=100&_=" + cacheBuster();

    if (!searchText.empty()) {
        endpoint += "&search=" + searchText;
    }

    auto res = api.get(endpoint);

    if (res.status_code != 200) {
        std::cout << "Search products failed: " << res.text << std::endl;
        return json::array();
    }

    try {
        auto response = json::parse(res.text);

        if (response.contains("data") && response["data"].is_array()) {
            return response["data"];
        }

        if (response.is_array()) {
            return response;
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Parse search products failed: " << ex.what() << std::endl;
    }

    return json::array();
}

std::string ProductService::getProductNameByBarcode(
    const std::string& barcode
)
{
    json products = searchProducts(barcode);

    for (const auto& product : products) {
        std::string productBarcode =
            product.value("barcode", "");

        if (productBarcode == barcode) {
            return product.value("name", barcode);
        }
    }

    return barcode;
}

bool ProductService::scanIn(
    const std::string& barcode,
    int quantity
)
{
    json body = {
        {"barcode", barcode},
        {"quantity", quantity}
    };

    auto res = api.post("/products/scan-in", body.dump());

    if (res.status_code != 200) {
        std::cout << "Scan in failed: " << res.text << std::endl;
        return false;
    }

    return true;
}

bool ProductService::scanOut(
    const std::string& barcode,
    int quantity
)
{
    json body = {
        {"barcode", barcode},
        {"quantity", quantity}
    };

    auto res = api.post("/products/scan-out", body.dump());

    if (res.status_code != 200) {
        std::cout << "Scan out failed: " << res.text << std::endl;
        return false;
    }

    return true;
}

json ProductService::getLowStockProducts()
{
    auto res = api.get("/products/low-stock?_=" + cacheBuster());

    if (res.status_code != 200) {
        std::cout << "Get low stock products failed: " << res.text << std::endl;
        return json::array();
    }

    try {
        auto response = json::parse(res.text);

        if (response.contains("data") && response["data"].is_array()) {
            return response["data"];
        }

        if (response.is_array()) {
            return response;
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Parse low stock products failed: " << ex.what() << std::endl;
    }

    return json::array();
}