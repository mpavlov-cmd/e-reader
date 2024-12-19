#include "IntentBook.h"

IntentBook::IntentBook(GxEPD2_GFX &display, TextIndex& textIndex, FileManager& fileManager) : AbstractDisplayIntent(display),
    textIndex(textIndex), fileManager(fileManager)
{
}

void IntentBook::onStartUp(IntentArgument arg)
{
    Serial.printf("Book intent started with arg: %s\n", arg.strValue);

    // Index Book
    TextIndexConf conf{
        (uint16_t) (textBox.width - textBox.padding),
        (uint16_t) (textBox.height - textBox.padding), 
        50,
        true
    };

    // Configure and run text index
    // TODO: Finish here, create indexing flag and check it on update. If indexing is done open book
    textIndex.configure(conf);
    String textIndexDirPath = textIndex.generateIdx(arg.strValue);
    Serial.printf("-- Index Generated! Dir: %s --\n", textIndexDirPath);

    Set<FileIndex> fileIndex(1024);
    fileManager.indexDirectory(textIndexDirPath.c_str(), DirIndexConf::FULL, fileIndex);

    if (fileIndex.size() == 0) {
        Serial.println("-- Fix Me --");
        return;
    }

    FileIndex* firstPage = fileIndex.getItem(0);
    Serial.printf("First page file name: %s\n", firstPage->getName());
    // File firstPage = fileManager.openFile()

    File firstPageFile = fileManager.openFile(firstPage->getPath(), FILE_READ);
	String firstPageString = "";
	while (firstPageFile.available())
	{
		char tc = firstPageFile.read();
		firstPageString.concat(tc);
	}
	
	firstPageFile.close();

	Serial.println("Page:");
	Serial.println(firstPageString);

    ModelText modelTextObj = {textBox, CENTER_CENTER, firstPageString};
    modelText = &modelTextObj;

    // Serial.pintf does not work with Srings, it will work as text.c_str()
    // Serial.printf("--- Model text inside of statup: %s ---- \n", modelTextObj.text.c_str());
    widgetText = new WidgetText(display);
    widgetText->upgrade(*modelText);
}

void IntentBook::onFrequncy()
{
    Serial.println("IntentBook on frequency");
}

void IntentBook::onExit()
{
    Serial.println("IntentBook on exit");
}

ActionResult IntentBook::onAction(ActionArgument arg)
{
    // In case input hed -> go home
    if (arg.held) {
        return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
    }

    return ActionResult::VOID;
}

uint8_t IntentBook::getId()
{
    return ID;
}
