#include <string>
#include <list>
#include <vector>
#include <inttypes.h>
#include <assert.h>
#include <unordered_set>
#include <dlib/geometry/rectangle.h>
#include <dlib/image_transforms.h>


#include "Calculation.h"
#include "SystemDrawingService.h"
#include "helpers.h"

#include "RecognitionService.h"

#include <string>
#include <iostream>

int main()
{
    std::string imgFileName = "resources/test_img.png";
    std::string textmap_name = "resources/textmap.txt";
    std::string linkmap_name = "resources/linkmap.txt";

    RecognitionService service(imgFileName,
                               textmap_name,
                               linkmap_name);

    auto rectangles = service.getFragmentsCraft();

    SystemDrawingService::drawRectangles(imgFileName,
                                         "craft_rectangles.png",
                                         dlib::bgr_pixel(0, 255, 0),
                                         rectangles);

    return 0;
}
