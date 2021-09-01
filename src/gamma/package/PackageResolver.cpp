#include "PackageResolver.hpp"
#include <stdexcept>

namespace gamma {

std::string PackageResolver::ResolveNode(const std::string& version) {

    // TODO: handle binary arch
    if (version == "latest" || version == "lts") {
        // we need some extra, and mildly annoying, additional checks for these lot.
        // The concept of "latest" in general requires determining what the newest version is.
        // LTS is worse because LTS is a human-defined concept that machines can't infer.
        //
        // I mean, is /en/download/latest and /en/download/lts really that much to ask for?
        // ... well, yes. It's JavaScript, it fantastic at doing everything awfully.
        cpr::Url url = {
            std::string("https://nodejs.org/en/download/") + (version == "latest" ? "current" : "")
        };
        cpr::Response r = cpr::Get(
            url
        );
        if (r.status_code != 200) {
            throw std::runtime_error("nodejs.org returned bad status code.");
        }
        // This part is really simple; we just need to find the version.
        // We leave URL validation to InstallationResolver
        std::string version = r.text.find()
    } else {
        return std::string("https://nodejs.org/dist/v") + version + "/node-v" + version + ".tar.gz";
    }
}

}
