#include "ivectorextraction.h"

IvectorExtraction::IvectorExtraction(bool derived_in) : derived_in(derived_in),
                                                        extractor() { }

IvectorExtraction::~IvectorExtraction() { }

void IvectorExtraction::SetModels(string femaleIvecMdl, string femaleUbm, string maleIvecMdl, string maleUbm) {
    this->femaleIvecMdl = femaleIvecMdl;
    this->maleIvecMdl = maleIvecMdl;
    this->femaleUbm = femaleUbm;
    this->maleUbm = maleUbm;

    {
        bool binary_in;
        Input ki(maleIvecMdl, &binary_in);
        extractor.Read(ki.Stream(), binary_in, derived_in);
        gender = male;
    }
    {
          bool binary_read;
          Input ki(maleUbm, &binary_read);
          maleFgmm.Read(ki.Stream(), binary_read);
    }
    {
          bool binary_read;
          Input ki(femaleUbm, &binary_read);
          femaleFgmm.Read(ki.Stream(), binary_read);
    }
}

void IvectorExtraction::SetGender(Gender gender) {
    if (gender == this->gender) {
        // Do nothing
        return;
    } else {
        // Set the gender and change model (because we don't have enough memory now)
        this->gender = gender;
        bool binary_in;
        if (gender == male) {
            Input ki(maleIvecMdl, &binary_in);
            extractor.Read(ki.Stream(), binary_in, derived_in);
        } else {
            Input ki(femaleIvecMdl, &binary_in);
            extractor.Read(ki.Stream(), binary_in, derived_in);
        }
    }
}
