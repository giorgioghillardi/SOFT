/**
 * Radiation :: Output :: Image
 *
 * Generate an output image file.
 */

#include <omp.h>
#include <softlib/config.h>
#include "Tools/Radiation/Output/Image.h"

using namespace __Radiation;

slibreal_t *Image::global_image = nullptr,
           *Image::global_imageQ = nullptr,
           *Image::global_imageU = nullptr,
           *Image::global_imageV = nullptr;

/**
 * Finish gathering output and merge output
 * stored on individual threads to the global
 * image.
 */
void Image::Finish() {
    #pragma omp critical (Image_Finish)
    {
        if (global_image == nullptr) {
            global_image = this->image;
            
            if (this->MeasuresPolarization()) {
                global_imageQ = this->imageQ;
                global_imageU = this->imageU;
                global_imageV = this->imageV;
            }
        } else {
            for (int i = 0; i < this->ntotpixels; i++)
                global_image[i] += this->image[i];

            if (this->MeasuresPolarization()) {
                for (int i = 0; i < this->ntotpixels; i++) {
                    global_imageQ[i] += this->imageQ[i];
                    global_imageU[i] += this->imageU[i];
                    global_imageV[i] += this->imageV[i];
                }
            }
            delete [] this->image;
        }
    }
}

/**
 * Generate the output file.
 * Called on the root thread only.
 */
void Image::Generate() {
    SFile *sf = SFile::Create(this->output, SFILE_MODE_WRITE);
    slibreal_t detpos[3], detdir[3], detvisang;

    detector->GetPosition().ToArray(detpos);
    detector->GetDirection().ToArray(detdir);
    detvisang = 2.0 * detector->GetVisionAngleFOV();

    slibreal_t **img = new slibreal_t*[this->nrowpixels];
    for (int i = 0; i < this->nrowpixels; i++)
        img[i] = global_image+(i*this->ncolpixels);

    if (!this->MeasuresPolarization())
        sf->WriteArray("image", img, this->nrowpixels, this->ncolpixels);
    else {
        slibreal_t **imgQ = new slibreal_t*[this->nrowpixels];
        slibreal_t **imgU = new slibreal_t*[this->nrowpixels];
        slibreal_t **imgV = new slibreal_t*[this->nrowpixels];

        for (int i = 0; i < this->nrowpixels; i++) {
            imgQ[i] = global_imageQ+(i*this->ncolpixels);
            imgU[i] = global_imageU+(i*this->ncolpixels);
            imgV[i] = global_imageV+(i*this->ncolpixels);
        }

        sf->WriteArray("StokesI", img, this->nrowpixels, this->ncolpixels);
        sf->WriteArray("StokesQ", imgQ, this->nrowpixels, this->ncolpixels);
        sf->WriteArray("StokesU", imgU, this->nrowpixels, this->ncolpixels);
        sf->WriteArray("StokesV", imgV, this->nrowpixels, this->ncolpixels);
    }

    sf->WriteList("detectorPosition", detpos, 3);
    sf->WriteList("detectorDirection", detdir, 3);
    sf->WriteList("detectorVisang", &detvisang, 1);

    // Include wall/separatrix data?
    unsigned int nwall = magfield->GetNDomain();
    slibreal_t **domain = new slibreal_t*[2], *rwall, *zwall;
    domain[0] = new slibreal_t[2*nwall];
    domain[1] = domain[0]+nwall;

    rwall = magfield->GetRDomain();
    zwall = magfield->GetZDomain();
    for (unsigned int i = 0; i < nwall; i++) {
        domain[0][i] = rwall[i];
        domain[1][i] = zwall[i];
    }

    sf->WriteArray("wall", domain, 2, nwall);

    sf->Close();

    delete [] domain[0];
    delete [] domain;

#ifdef COLOR_TERMINAL
    // Check if image is empty
    bool empty = true;
    for (int i = 0; i < this->nrowpixels; i++) {
        for (int j = 0; j < this->ncolpixels; j++) {
            if (img[i][j] != 0) {
                empty = false;
                break;
            }
        }
    }

    if (empty)
        SOFT::PrintInfo("Wrote \x1B[1;33mempty\x1B[0m image to '%s'.", this->output.c_str());
    else
#endif
        SOFT::PrintInfo("Wrote image to '%s'.", this->output.c_str());

    delete [] img;
    delete [] global_image;
}

