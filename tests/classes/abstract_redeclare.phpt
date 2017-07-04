--TEST--
ZE2 A method cannot be redeclared abstract
--FILE--
<?php

class pass
{
    function show()
    {
        echo "Call to function show()\n";
    }
}
class fail extends pass
{
    abstract function show();
}
function fn1261786230()
{
    echo "Done\n";
    // Shouldn't be displayed
}
fn1261786230();
--EXPECTF--

Fatal error: Class fail contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (fail::show) in %sabstract_redeclare.php on line %d
