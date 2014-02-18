#ifndef DENALI_VISUALIZE_H
#define DENALI_VISUALIZE_H

namespace denali {

    template <typename Landscape>
    class Visualizer
    {
        const Landscape& _landscape;
    public:

        Visualizer(const Landscape& landscape)
                : _landscape(landscape) { }
        

    };

}


#endif
