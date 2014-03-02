#include "mainwindow.h"

#include <denali/contour_tree.h>
#include <denali/fileio.h>
#include <denali/rectangular_landscape.h>

#include <sstream>

MainWindow::MainWindow()
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

    connect(this, SIGNAL(landscapeChanged()), this, SLOT(renderLandscape()));

    connect(this, SIGNAL(cellSelected(unsigned int)), 
            this, SLOT(updateCellSelection(unsigned int)));

    connect(_mainwindow.pushButtonExpand, SIGNAL(clicked()),
            this, SLOT(expandTree()));

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
    emit cellSelected(cell);
}


void MainWindow::updateCellSelection(unsigned int cell)
{
    // get the parent and child nodes of the selected component
    size_t parent, child;
    _landscape_context->getComponent(cell, parent, child);

    double parent_value = _landscape_context->getValue(parent);
    double child_value  = _landscape_context->getValue(child);

    std::stringstream message;
    message << "<b>Component Selected</b>: " 
            << parent << " &#8594; " 
            << child << "<br>";

    message << "<b>Parent value: </b>" << parent_value << "<br>";
    message << "<b>Child value:  </b>" << child_value << "<br>";

    setStatus(message.str());
}


void MainWindow::expandTree()
{
    if (!_landscape_context) return;

    std::cout << "Expanding tree..." << std::endl;
    _landscape_context->expand();
    emit landscapeChanged();
}
