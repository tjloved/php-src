--TEST--
ZE2 factory objects
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Circle
{
    function draw()
    {
        echo "Circle\n";
    }
}
class Square
{
    function draw()
    {
        print "Square\n";
    }
}
function ShapeFactoryMethod($shape)
{
    switch ($shape) {
        case "Circle":
            return new Circle();
        case "Square":
            return new Square();
    }
}
function fn438553298()
{
    ShapeFactoryMethod("Circle")->draw();
    ShapeFactoryMethod("Square")->draw();
}
fn438553298();
--EXPECT--
Circle
Square
