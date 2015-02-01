#include "NLength.hpp"


NLength::NLength() {}


NLength::NLength(float pixels, float fraction, float dips)
    : mPixels(pixels), mFraction(fraction), mDips(dips) {}


float NLength::Evaluate(float parentNLength, float dpi) const {
  return mPixels + parentNLength * mFraction + mDips * dpi / 96.0f;
}


NLength NLength::operator-(const NLength &other) const {
  return NLength(mPixels - other.mPixels, mFraction - other.mFraction, mDips - other.mDips);
}


NLength NLength::operator+(const NLength &other) const {
  return NLength(mPixels + other.mPixels, mFraction + other.mFraction, mDips + other.mDips);
}


NLength NLength::operator*(float factor) const {
  return NLength(mPixels * factor, mFraction * factor, mDips * factor);
}


NLength NLength::operator/(float factor) const {
  return NLength(mPixels / factor, mFraction / factor, mDips / factor);
}


bool NLength::operator==(const NLength &other) const {
  return other.mDips == mDips && other.mFraction == mFraction && other.mPixels == mPixels;
}


bool NLength::operator!=(const NLength &other) const {
  return other.mDips != mDips || other.mFraction != mFraction || other.mPixels != mPixels;
}
