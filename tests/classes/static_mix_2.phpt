--TEST--
ZE2 You cannot overload a non static method with a static method
--FILE--
<?php

class pass
{
    function show()
    {
        echo "Call to function pass::show()\n";
    }
}
class fail extends pass
{
    static function show()
    {
        echo "Call to function fail::show()\n";
    }
}
function fn239870185()
{
    $t = new pass();
    $t->show();
    fail::show();
    echo "Done\n";
    // shouldn't be displayed
}
fn239870185();
--EXPECTF--
Fatal error: Cannot make non static method pass::show() static in class fail in %s on line %d
