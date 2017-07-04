--TEST--
Bug #30820 (static member conflict with $this->member silently ignored)
--INI--
error_reporting=4095
opcache.optimization_level=0
--FILE--
<?php

class Blah
{
    private static $x;
    public function show()
    {
        Blah::$x = 1;
        $this->x = 5;
        // no warning, but refers to different variable
        echo 'Blah::$x = ' . Blah::$x . "\n";
        echo '$this->x = ' . $this->x . "\n";
    }
}
function fn856586184()
{
    $b = new Blah();
    $b->show();
}
fn856586184();
--EXPECTF--
Notice: Accessing static property Blah::$x as non static in %sbug30820.php on line %d
Blah::$x = 1

Notice: Accessing static property Blah::$x as non static in %sbug30820.php on line %d
$this->x = 5
