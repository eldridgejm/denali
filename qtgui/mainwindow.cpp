#include "mainwindow.h"

#include <QProcess>

#include <denali/contour_tree.h>
#include <denali/fileio.h>
#include <denali/rectangular_landscape.h>

#include <cmath>
#include <sstream>

MainWindow::MainWindow() :
    _max_persistence_slider_value(100)
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

    connect(_mainwindow.radioButtonRootMin, SIGNAL(clicked(bool)),
            this, SLOT(changeLandscapeRoot()));

    connect(_mainwindow.radioButtonRootMax, SIGNAL(clicked(bool)),
            this, SLOT(changeLandscapeRoot()));

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

    connect(this, SIGNAL(cellSelected(unsigned int)),
            this, SLOT(cellSelectionCallback(unsigned int)));

    connect(_mainwindow.pushButtonTreeBuilder, SIGNAL(clicked()),
            this, SLOT(treeBuilderCallback()));

    connect(this, SIGNAL(landscapeChanged()),
            this, SLOT(enableLoadWeightMap()));

    connect(_mainwindow.actionLoad_Weight_Map, SIGNAL(triggered()),
            this, SLOT(loadWeightMapFile()));
}


void MainWindow::setContext(LandscapeContext* context)
{
    _landscape_context = boost::shared_ptr<LandscapeContext>(context);
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

    // read the contour tree file
    ContourTree* contour_tree;
    contour_tree = new ContourTree(denali::readContourTreeFile(filename.c_str()));

    // wrap them in a context
    Context* context = new Context(contour_tree);
    this->setContext(context);

    // choose the root of the landscape
    this->changeLandscapeRoot();

    // update the persistence slider
    this->enablePersistenceSlider();

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
    _landscape_interface->buildLandscapeMesh(*_landscape_context);
}


void MainWindow::changeLandscapeRoot()
{
    // check that there is actually a context
    if (!_landscape_context) return;

    size_t root_id;
    if (_mainwindow.radioButtonRootMin->isChecked()) 
    {
        root_id = _landscape_context->getMinLeafID();
    } 
    else if (_mainwindow.radioButtonRootMax->isChecked()) 
    {
        root_id = _landscape_context->getMaxLeafID();
    }

    if (_landscape_context->isValid())
    {
        size_t current_root = _landscape_context->getRootID();
        if (current_root == root_id) return;
    }

    std::stringstream message;
    message << "The landscape is now rooted at node " << root_id << ".";
    this->setStatus(message.str());

    _landscape_context->buildLandscape(root_id);

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
    std::cout << "Refining subtree..." << std::endl;
    // get the parent and child nodes of the selection
    size_t parent, child;
    _landscape_context->getComponentParentChild(_cell_selection, parent, child);

    // get the current persistence value
    int value = _mainwindow.horizontalSliderPersistence->value();
    double persistence = ((double) value)/_max_persistence_slider_value * 
            _landscape_context->getMaxPersistence();

    _landscape_context->simplifySubtreeByPersistence(parent, child, persistence);

    // we need to rebuild the landscape
    size_t root = _landscape_context->getMinLeafID();
    // size_t root = _landscape_context->getRootID();
    _landscape_context->buildLandscape(root);
    
    emit landscapeChanged();
}


void MainWindow::cellSelectionCallback(unsigned int cell)
{
    std::stringstream procname;
    size_t parent, child;
    _landscape_context->getComponentParentChild(_cell_selection, parent, child);
    procname << "./testproc.py " << parent << " " << child << std::endl;
    QProcess process;
    process.start(procname.str().c_str());
    process.waitForFinished(-1);

    QString p_stdout = process.readAllStandardOutput();
    this->appendStatus(p_stdout.toUtf8().constData());
};


void MainWindow::treeBuilderCallback()
{
    typedef denali::ContourTree ContourTree;
    typedef ConcreteLandscapeContext
            <ContourTree, denali::RectangularLandscapeBuilder> Context;

    std::cout << "Running treebuilder..." << std::endl;
    
    QProcess process;
    process.start("./buildtree.sh");
    process.waitForFinished(-1);

    QString p_stdout = process.readAllStandardOutput();
    std::istringstream readtree(p_stdout.toUtf8().constData());

    // now read in the contour tree
    ContourTree* contour_tree;
    contour_tree = new ContourTree(denali::readContourTreeFromStream(readtree));

    // wrap them in a context
    Context* context = new Context(contour_tree);
    this->setContext(context);

    // choose the root of the landscape
    this->changeLandscapeRoot();

    // update the persistence slider
    this->enablePersistenceSlider();

    // notify the user that all is well
    std::stringstream message;
    message << "Contour tree callback was run." << std::endl;
    this->appendStatus(message.str());
}


void MainWindow::enableLoadWeightMap()
{
    _mainwindow.actionLoad_Weight_Map->setEnabled(true);
}


void MainWindow::loadWeightMapFile()
{
    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Open Weight Map File"), "", tr("Files(*.weights)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    // read the contour tree file
    denali::WeightMap* weight_map = new denali::WeightMap;
    denali::readWeightMapFile(filename.c_str(), *weight_map);

    _landscape_context->setWeightMap(weight_map);
    _landscape_context->buildLandscape(_landscape_context->getRootID());
    
    emit landscapeChanged();

}


