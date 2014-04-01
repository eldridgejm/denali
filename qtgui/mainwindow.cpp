#include "mainwindow.h"

#include <QProcess>
#include <QTemporaryFile>

#include <denali/contour_tree.h>
#include <denali/fileio.h>
#include <denali/rectangular_landscape.h>

#include <cmath>
#include <sstream>

MainWindow::MainWindow() :
    _max_persistence_slider_value(100),
    _color_map_dialog(new ColorMapDialog(this)),
    _callbacks_dialog(new CallbacksDialog(this)),
    _choose_root_dialog(new ChooseRootDialog(this)),
    _use_color_map(false), _progress_wait_time(300)
{
    // set up the user inteface
    _mainwindow.setupUi(this);

    // create a new landscape interface and connect it to the vtk widget
    _landscape_interface = boost::shared_ptr<LandscapeInterface>(
            new LandscapeInterface(_mainwindow.qvtkWidget->GetRenderWindow()));

    // register this as an observer of the landscape
    _landscape_interface->registerEventObserver(this);

    connect(_mainwindow.actionOpen_Tree, SIGNAL(triggered()), 
            this, SLOT(openContourTreeFile()));

    connect(this, SIGNAL(landscapeChanged()), 
            this, SLOT(renderLandscape()));

    connect(this, SIGNAL(cellSelected(unsigned int)), 
            this, SLOT(updateCellSelection(unsigned int)));

    connect(_mainwindow.horizontalSliderPersistence, SIGNAL(valueChanged(int)),
            this, SLOT(updatePersistence(int)));

    connect(this, SIGNAL(cellSelected(unsigned int)), 
            this, SLOT(enableRefineSubtree()));

    // changing the landscape may invalidate the selection, making the refine button
    // invalid
    connect(this, SIGNAL(landscapeChanged()),
            this, SLOT(disableRefineSubtree()));

    connect(_mainwindow.pushButtonRefineSubtree, SIGNAL(clicked()),
            this, SLOT(refineSubtree()));

    // Weight maps
    ////////////////////////////////////////////////////////////////////////////

    connect(this, SIGNAL(landscapeChanged()),
            this, SLOT(enableLoadWeightMap()));

    connect(_mainwindow.actionLoad_Weight_Map, SIGNAL(triggered()),
            this, SLOT(loadWeightMapFile()));

    connect(_mainwindow.actionClear_Weight_Map, SIGNAL(triggered()),
            this, SLOT(clearWeightMap()));

    // Color maps
    ////////////////////////////////////////////////////////////////////////////

    connect(this, SIGNAL(landscapeChanged()),
            this, SLOT(enableConfigureColorMap()));

    connect(_mainwindow.actionConfigure_Color_Map, SIGNAL(triggered()),
            this, SLOT(configureColorMap()));

    connect(_mainwindow.actionClear_Color_Map, SIGNAL(triggered()),
            this, SLOT(clearColorMap()));

    // Callbacks
    ////////////////////////////////////////////////////////////////////////////

    connect(_mainwindow.actionConfigure_Callbacks, SIGNAL(triggered()),
            this, SLOT(configureCallbacks()));

    connect(_mainwindow.pushButtonInfoCallback, SIGNAL(clicked()),
            this, SLOT(runInfoCallback()));

    connect(_mainwindow.pushButtonTreeCallback, SIGNAL(clicked()),
            this, SLOT(runTreeCallback()));

    connect(_mainwindow.pushButtonVoidCallback, SIGNAL(clicked()),
            this, SLOT(runVoidCallback()));

    connect(this, SIGNAL(cellSelected(unsigned int)), 
            this, SLOT(runCallbacksOnSelection()));

    // Rebasing
    ////////////////////////////////////////////////////////////////////////////

    connect(this, SIGNAL(cellSelected(unsigned int)),
            this, SLOT(enableRebaseLandscape()));

    connect(this, SIGNAL(landscapeChanged()),
            this, SLOT(disableRebaseLandscape()));

    connect(_mainwindow.pushButtonRebase, SIGNAL(clicked()),
            this, SLOT(rebaseLandscape()));

    // Expand
    ////////////////////////////////////////////////////////////////////////////

    connect(_mainwindow.pushButtonExpand, SIGNAL(clicked()),
            this, SLOT(expandLandscape()));

    connect(_mainwindow.actionExit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(this, SIGNAL(landscapeChanged()),
            this, SLOT(enableExpandLandscape()));

    // Choose root
    ////////////////////////////////////////////////////////////////////////////

    connect(this, SIGNAL(landscapeChanged()), 
            this, SLOT(enableChooseRoot()));

    connect(_mainwindow.pushButtonChooseRoot, SIGNAL(clicked()),
            this, SLOT(chooseRoot()));
}


void MainWindow::setContext(LandscapeContext* context)
{
    _landscape_context = boost::shared_ptr<LandscapeContext>(context);

    // invalidate the color map
    _use_color_map = false;
    _landscape_context->setColorMap(boost::shared_ptr<denali::ColorMap>());
    _landscape_context->setColorReduction(boost::shared_ptr<Reduction>());

    disableClearColorMap();
    disableClearWeightMap();

    // choose the root of the landscape to be the minimum, by default
    _choose_root_dialog->setMinimumNodeChecked(true);
    this->changeLandscapeRoot();

    // update the persistence slider
    this->enablePersistenceSlider();

    this->updateCallbackAvailability();

    this->disableRebaseLandscape();
}


denali::ContourTree* readAndAllocateContourTree(std::string filename)
{
    return new denali::ContourTree(denali::readContourTreeFile(filename.c_str()));
}


void MainWindow::openContourTreeFile()
{
    typedef denali::ContourTree ContourTree;
    typedef ConcreteLandscapeContext
            <ContourTree, denali::RectangularLandscapeBuilder> Context;

    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Open Contour Tree File"), "", tr("Files(*.tree)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    // if there was no file specified, do nothing
    if (filename.size() == 0) return;

    // read the contour tree file
    ContourTree* contour_tree = NULL;
    try
    {
        QProgressDialog progress;
        progress.setLabelText("Reading contour tree...");
        progress.setRange(0,0);
        progress.setCancelButton(0);

        // only show the progress bar if some time has passed
        QTimer::singleShot(_progress_wait_time, &progress, SLOT(show()));

        QFuture<ContourTree*> tree_future = 
                QtConcurrent::run(readAndAllocateContourTree, filename);

        while (!tree_future.isFinished())
        {
            QApplication::processEvents();
        }

        contour_tree = tree_future.result();

    }
    catch (std::exception& e)
    {
        QString message("There was a problem reading the contour tree file.");

        QMessageBox msgbox;
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText(message);
        msgbox.exec();

        delete contour_tree;

        return;
    }

    // update the filename
    _filename = filename;

    // wrap them in a context
    Context* context = new Context(contour_tree);
    this->setContext(context);

    // notify the user that all is well
    std::stringstream message;
    message << "Contour tree file <i>\"" << filename << "\"</i> read.";
    this->appendStatus(message.str());
}


void MainWindow::setStatus(const std::string& s)
{
    _mainwindow.textEditStatusBox->setHtml(s.c_str());
}


void MainWindow::appendStatus(const std::string& s)
{
    _mainwindow.textEditStatusBox->append(s.c_str());
}


void MainWindow::renderLandscape()
{
    // check that there is actually a context
    if (!_landscape_context) return;

    // now update the display
    _landscape_interface->renderLandscape(*_landscape_context, _use_color_map);
}


void MainWindow::changeLandscapeRoot()
{
    // check that there is actually a context
    if (!_landscape_context) return;

    size_t root_id;
    if (_choose_root_dialog->isMinimumNodeChecked()) 
    {
        root_id = _landscape_context->getMinNodeID();
    } 
    else if (_choose_root_dialog->isMaximumNodeChecked()) 
    {
        root_id = _landscape_context->getMaxNodeID();
    }
    else
    {
        root_id = _choose_root_dialog->getOtherNode();
    }

    std::stringstream message;
    message << "The landscape is now rooted at node " << root_id << ".";
    this->setStatus(message.str());

    QFuture<void> result = 
            QtConcurrent::run(&*_landscape_context, &LandscapeContext::buildLandscape, root_id);

    QProgressDialog progress;
    progress.setLabelText("Building landscape...");
    progress.setRange(0,0);
    progress.setCancelButton(0);

    // only show the progress bar if some time has passed
    QTimer::singleShot(_progress_wait_time, &progress, SLOT(show()));

    while (!result.isFinished())
    {
        QApplication::processEvents();
    }

    emit landscapeChanged();
}


void MainWindow::receiveCellSelection(unsigned int cell)
{
    _cell_selection = cell;
    emit cellSelected(cell);
}


void MainWindow::updateCellSelection(unsigned int cell)
{
    // get the parent and child nodes of the selected component
    size_t parent, child;
    _landscape_context->getComponentParentChild(cell, parent, child);

    double parent_value = _landscape_context->getValue(parent);
    double child_value  = _landscape_context->getValue(child);

    double component_weight = _landscape_context->getComponentWeight(cell);
    double parent_tot_weight = _landscape_context->getTotalNodeWeight(parent);
    double child_tot_weight = _landscape_context->getTotalNodeWeight(child);

    std::stringstream message;
    message << "<b>Component Selected</b>: " 
            << parent << " &#8594; " 
            << child << "<br>";

    message << "<b>Parent value: </b>" << parent_value << "<br>";
    message << "<b>Child value:  </b>" << child_value << "<br>";
    message << "<b>Persistence: </b>" << fabs(parent_value - child_value) << "<br>";
    message << "<b>Component weight: </b>" << component_weight << "<br>";
    message << "<b>Parent total weight: </b>" << parent_tot_weight << "<br>";
    message << "<b>Child total weight: </b>" << child_tot_weight << "<br>";

    if (_landscape_context->hasReduction()) {
        unsigned int cid = _landscape_context->getComponentIdentifierFromTriangle(cell);
        double reduction_value = _landscape_context->getComponentReductionValue(cid);
        message << "<b>Reduction value: </b>" << reduction_value << "<br>";
    }

    setStatus(message.str());
}


void MainWindow::enablePersistenceSlider()
{
    _mainwindow.horizontalSliderPersistence->setEnabled(true);
    _mainwindow.horizontalSliderPersistence->setMinimum(0);
    _mainwindow.horizontalSliderPersistence->setMaximum(_max_persistence_slider_value);
    _mainwindow.horizontalSliderPersistence->setSliderPosition(0);
    this->updatePersistence(0);
}

void MainWindow::updatePersistence(int value)
{
    // compute the persistence level
    double persistence = ((double) value)/_max_persistence_slider_value * 
            _landscape_context->getMaxPersistence();
    
    // set the label to this persistence
    std::stringstream label;
    label << persistence;
    _mainwindow.labelPersistence->setText(label.str().c_str());
}


void MainWindow::enableRefineSubtree()
{
    _mainwindow.pushButtonRefineSubtree->setEnabled(true);
}

void MainWindow::disableRefineSubtree()
{
    _mainwindow.pushButtonRefineSubtree->setEnabled(false);
}

void MainWindow::refineSubtree()
{
    // get the parent and child nodes of the selection
    size_t parent, child;
    _landscape_context->getComponentParentChild(_cell_selection, parent, child);

    // get the current persistence value
    int value = _mainwindow.horizontalSliderPersistence->value();
    double persistence = ((double) value)/_max_persistence_slider_value * 
            _landscape_context->getMaxPersistence();

    _landscape_context->simplifySubtreeByPersistence(parent, child, persistence);

    // we need to rebuild the landscape
    changeLandscapeRoot();
}




void MainWindow::enableLoadWeightMap()
{
    _mainwindow.actionLoad_Weight_Map->setEnabled(true);
}


void MainWindow::enableClearWeightMap()
{
    _mainwindow.actionClear_Weight_Map->setEnabled(true);
}


void MainWindow::disableClearWeightMap()
{
    _mainwindow.actionClear_Weight_Map->setEnabled(false);
}


void MainWindow::loadWeightMapFile()
{
    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Open Weight Map File"), "", tr("Files(*.weights)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    // if the user didn't specify a file, do nothing
    if (filename.size() == 0) return;

    // read the weight file
    boost::shared_ptr<denali::WeightMap> weight_map = 
            boost::shared_ptr<denali::WeightMap>(new denali::WeightMap);

    try 
    {
        denali::readWeightMapFile(filename.c_str(), *weight_map);
    }
    catch (std::exception& e)
    {
        QString message = QString::fromStdString(
                std::string("There was a problem reading the weight file: ") + 
                e.what());

        QMessageBox msgbox;
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText(message);
        msgbox.exec();
        return;
    }

    _landscape_context->setWeightMap(weight_map);
    _landscape_context->buildLandscape(_landscape_context->getRootID());

    // now that there is a weight map, it can be cleared
    enableClearWeightMap();
    
    emit landscapeChanged();
}


void MainWindow::clearWeightMap()
{
    boost::shared_ptr<denali::WeightMap> null_map;
    _landscape_context->setWeightMap(null_map);

    disableClearWeightMap();

    _landscape_context->buildLandscape(_landscape_context->getRootID());

    emit landscapeChanged();
}


void MainWindow::enableConfigureColorMap()
{
    _mainwindow.actionConfigure_Color_Map->setEnabled(true);
}


void MainWindow::enableClearColorMap()
{
    _mainwindow.actionClear_Color_Map->setEnabled(true);
}


void MainWindow::disableClearColorMap()
{
    _mainwindow.actionClear_Color_Map->setEnabled(false);
}


void MainWindow::configureColorMap()
{
    // the reduction is automatically recalculated if a new color map 
    // or reduction functor is specified, and the other is non-null.
    // we set both to be null to prevent this from happening.
    _landscape_context->setColorMap(boost::shared_ptr<denali::ColorMap>());
    _landscape_context->setColorReduction(boost::shared_ptr<Reduction>());

    if (_color_map_dialog->exec() == QDialog::Accepted)
    {
        std::string filename = _color_map_dialog->getColorMapPath();

        // if no file specified, do nothing
        if (filename.size() == 0) return;

        // read the color file
        boost::shared_ptr<denali::ColorMap> color_map = 
                boost::shared_ptr<denali::ColorMap>(new denali::ColorMap);

        try
        {
            denali::readColorMapFile(filename.c_str(), *color_map);
        }
        catch (std::exception& e)
        {
            QString message = QString::fromStdString(
                    std::string("There was a problem reading the color file: ") + 
                    e.what());

            QMessageBox msgbox;
            msgbox.setIcon(QMessageBox::Warning);
            msgbox.setText(message);
            msgbox.exec();
            return;
        }

        int reduction_id = _color_map_dialog->getReductionIndex(); 
        boost::shared_ptr<Reduction> reduction;

        switch(reduction_id)
        {
            case MAXIMUM:
                reduction = boost::shared_ptr<Reduction>(new MaxReduction);
                break;

            case MINIMUM:
                reduction = boost::shared_ptr<Reduction>(new MinReduction);
                break;

            case MEAN:
                reduction = boost::shared_ptr<Reduction>(new MeanReduction);
                break;

            case COUNT:
                reduction = boost::shared_ptr<Reduction>(new CountReduction);
                break;

            case VARIANCE:
                reduction = boost::shared_ptr<Reduction>(new VarianceReduction);
                break;

            case COVARIANCE:
                reduction = boost::shared_ptr<Reduction>(new CovarianceReduction);
                break;

            case CORRELATION:
                reduction = boost::shared_ptr<Reduction>(new CorrelationReduction);
                break;

            default:
                reduction = boost::shared_ptr<Reduction>(new MaxReduction);
        }

        // determine what parts of a component to include in the reduction
        _landscape_context->setParentInReduction(
                _color_map_dialog->isParentContributorChecked());

        _landscape_context->setChildInReduction(
                _color_map_dialog->isChildContributorChecked());

        if (_color_map_dialog->isBothContributorChecked())
        {
            _landscape_context->setParentInReduction(true);
            _landscape_context->setChildInReduction(true);
        }

        _landscape_context->setMembersInReduction(
                _color_map_dialog->includeMembers());

        try 
        {
            _landscape_context->setColorMap(color_map);
            _landscape_context->setColorReduction(reduction);
        }
        catch (std::exception& e)
        {
            QString message = QString::fromStdString(
                    std::string("There was a problem loading the color map: ") + 
                    e.what());

            QMessageBox msgbox;
            msgbox.setIcon(QMessageBox::Warning);
            msgbox.setText(message);
            msgbox.exec();

            // invalidate any traces of the color map
            _use_color_map = false;
            _landscape_context->setColorMap(boost::shared_ptr<denali::ColorMap>());
            _landscape_context->setColorReduction(boost::shared_ptr<Reduction>());

            disableClearColorMap();
            return;
        }

        // this isn't very elegant, but correlation color maps should have their
        // ranges setto -1 to 1
        if (reduction_id == CORRELATION) 
        {
            _landscape_context->setMaxReductionValue(1);
            _landscape_context->setMinReductionValue(-1);
        }

        _mainwindow.actionClear_Color_Map->setEnabled(true);

        _use_color_map = true;
        renderLandscape();
    }

}


void MainWindow::clearColorMap()
{
    _use_color_map = false;
    _landscape_context->setColorMap(boost::shared_ptr<denali::ColorMap>());
    _landscape_context->setColorReduction(boost::shared_ptr<Reduction>());

    disableClearColorMap();

    renderLandscape();
}


void MainWindow::configureCallbacks()
{
    _callbacks_dialog->exec();
    this->updateCallbackAvailability();
}

void MainWindow::updateCallbackAvailability()
{
    if (_callbacks_dialog->getInfoCallback().size() != 0) {
        this->enableInfoCallback();
    } else {
        this->disableInfoCallback();
    }

    if (_callbacks_dialog->getTreeCallback().size() != 0) {
        this->enableTreeCallback();
    } else {
        this->disableTreeCallback();
    }

    if (_callbacks_dialog->getVoidCallback().size() != 0) {
        this->enableVoidCallback();
    } else {
        this->disableVoidCallback();
    }
}

std::string MainWindow::runCallback(
        std::string callback_path, 
        unsigned int cell,
        bool provide_subtree)
{

    size_t parent, child;
    _landscape_context->getComponentParentChild(_cell_selection, parent, child);

    double parent_value = _landscape_context->getValue(parent);
    double child_value = _landscape_context->getValue(child);

    std::stringstream command;
    command << callback_path;

    QTemporaryFile tempfile;
    if (!tempfile.open())
    {
        throw std::runtime_error("Problem opening tempfile.");
    }

    command << " " << tempfile.fileName().toUtf8().constData();

    if (_filename.size() > 0) {
        tempfile.write("# file\n");
        tempfile.write(_filename.c_str());
        tempfile.write("\n");
    }

    tempfile.write("# component\n");
    std::stringstream component_str;
    component_str << parent << " " << parent_value << std::endl;
    component_str << child << " " << child_value << std::endl;
    tempfile.write(component_str.str().c_str());

    LandscapeContext::Members members;

    LandscapeContext::Members arc_members = 
            _landscape_context->getMembers(parent, child);

    LandscapeContext::Members parent_members =
            _landscape_context->getMembers(parent);

    LandscapeContext::Members child_members =
            _landscape_context->getMembers(child);

    members.insert(arc_members.begin(), arc_members.end());
    members.insert(parent_members.begin(), parent_members.end());
    members.insert(child_members.begin(), child_members.end());

    tempfile.write("# members\n");
    for (LandscapeContext::Members::const_iterator it = members.begin();
            it != members.end(); ++it)
    {
        unsigned int id = it->first;
        double value = it->second;

        std::stringstream member_line;
        member_line << id << " " << value << std::endl;
        tempfile.write(member_line.str().c_str());
    }

    if (provide_subtree)
    {
        tempfile.write("# subtree\n");
        LandscapeContext::Members subtree_members = 
                _landscape_context->getSubtreeMembers(parent, child);

        for (LandscapeContext::Members::const_iterator it = subtree_members.begin();
                it != subtree_members.end(); ++it)
        {
            unsigned int id = it->first;
            double value = it->second;

            std::stringstream member_line;
            member_line << id << " " << value << std::endl;
            tempfile.write(member_line.str().c_str());
        }
    } 

    tempfile.close();

    QProcess process;
    process.start(command.str().c_str());
    process.waitForFinished(-1);

    QString p_stdout = process.readAllStandardOutput();
    QString p_stderr = process.readAllStandardError();

    if (p_stderr.size() > 0)
    {
        std::cerr << "Error while running callback: " << std::endl;
        std::cerr << p_stderr.toUtf8().constData() << std::endl;
    }

    return p_stdout.toUtf8().constData();
};


void MainWindow::enableInfoCallback() {
    _mainwindow.pushButtonInfoCallback->setEnabled(true);
}


void MainWindow::enableTreeCallback() {
    _mainwindow.pushButtonTreeCallback->setEnabled(true);
}


void MainWindow::enableVoidCallback() {
    _mainwindow.pushButtonVoidCallback->setEnabled(true);
}


void MainWindow::disableInfoCallback() {
    _mainwindow.pushButtonInfoCallback->setEnabled(false);
}


void MainWindow::disableTreeCallback() {
    _mainwindow.pushButtonTreeCallback->setEnabled(false);
}


void MainWindow::disableVoidCallback() {
    _mainwindow.pushButtonVoidCallback->setEnabled(false);
}


void MainWindow::runCallbacksOnSelection()
{
    if (_callbacks_dialog->runInfoOnSelection() && 
            _callbacks_dialog->getInfoCallback().size() != 0) {
        runInfoCallback();
    }

    if (_callbacks_dialog->runTreeOnSelection() && 
            _callbacks_dialog->getTreeCallback().size() != 0) {
        runTreeCallback();
    }

    if (_callbacks_dialog->runVoidOnSelection() && 
            _callbacks_dialog->getVoidCallback().size() != 0) {
        runVoidCallback();
    }
}

void MainWindow::runInfoCallback()
{
    std::string callback_path = _callbacks_dialog->getInfoCallback();
    bool provide_subtree = _callbacks_dialog->provideInfoSubtree();

    std::string result = runCallback(callback_path, _cell_selection, provide_subtree);
    if (result.size() > 0) {
        this->appendStatus(result);
    }
}


void MainWindow::runTreeCallback()
{
    typedef denali::ContourTree ContourTree;
    typedef ConcreteLandscapeContext
            <ContourTree, denali::RectangularLandscapeBuilder> Context;

    std::string callback_path = _callbacks_dialog->getTreeCallback();
    bool provide_subtree = _callbacks_dialog->provideTreeSubtree();

    std::string result = runCallback(callback_path, _cell_selection, provide_subtree);

    if (result.size() == 0) {
            QString message = QString::fromStdString(
                    std::string("The tree callback produced no output."));

            QMessageBox msgbox;
            msgbox.setIcon(QMessageBox::Warning);
            msgbox.setText(message);
            msgbox.exec();
            return;
    }

    std::stringstream readtree(result);

    // now read in the contour tree
    ContourTree* contour_tree;
    contour_tree = new ContourTree(denali::readContourTreeFromStream(readtree));

    // wrap them in a context
    Context* context = new Context(contour_tree);
    this->setContext(context);


}


void MainWindow::runVoidCallback()
{
    std::string callback_path = _callbacks_dialog->getVoidCallback();
    bool provide_subtree = _callbacks_dialog->provideVoidSubtree();

    runCallback(callback_path, _cell_selection, provide_subtree);
}


void MainWindow::enableRebaseLandscape()
{
    _mainwindow.pushButtonRebase->setEnabled(true);
}


void MainWindow::disableRebaseLandscape()
{
    _mainwindow.pushButtonRebase->setEnabled(false);
}


void MainWindow::rebaseLandscape()
{
    // get the parent and child nodes of the selection
    size_t parent, child;
    _landscape_context->getComponentParentChild(_cell_selection, parent, child);

    LandscapeContext* new_context = _landscape_context->rebaseLandscape(parent, child);

    this->setContext(new_context);

    emit landscapeChanged();
}


void MainWindow::enableExpandLandscape()
{
    _mainwindow.pushButtonExpand->setEnabled(true);
}


void MainWindow::disableExpandLandscape()
{
    _mainwindow.pushButtonExpand->setEnabled(false);
}


void MainWindow::expandLandscape()
{
    _landscape_context->expandLandscape();

    // we need to rebuild the landscape
    changeLandscapeRoot();
}


void MainWindow::enableChooseRoot()
{
    _mainwindow.pushButtonChooseRoot->setEnabled(true);
}


void MainWindow::chooseRoot()
{
    if (_choose_root_dialog->exec() == QDialog::Accepted)
    {
        // if a root other than min or max is specified, we have to verify 
        // that it exists
        if (_choose_root_dialog->isOtherNodeChecked())
        {
            unsigned int other_root = _choose_root_dialog->getOtherNode();

            if (!_landscape_context->isNodeValid(other_root))
            {
                QString message("The specified node is not in the landscape. Please choose another.");

                QMessageBox msgbox;
                msgbox.setIcon(QMessageBox::Warning);
                msgbox.setText(message);
                msgbox.exec();

                chooseRoot();

                // let the recursive call to chooseRoot actually change the 
                // landscape root
                return;
            }
        }

        // actually change the landscape root
        changeLandscapeRoot();
    }

}
