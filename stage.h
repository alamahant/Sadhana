#ifndef STAGE_H
#define STAGE_H

#include <QString>

struct Stage
{
    Stage();
    
    // Content
    QString name;           // "Opening", "Mantra", etc.
    QString liturgyText;    // For text area
    QString mantraText;     // For mantra label
    QString imagePath;      // For deity image
    QString audioPath;      // For background audio
    QString pdfPath;        // PDF file path
    
    // Display flags
   // bool usePdf = false;            // true = show PDF, false = show text
    bool showImage = true;         // true = show image, false = hide
    bool showMantra;        // true = show mantra, false = hide
    int lifetimeCount;
};

#endif // STAGE_H
