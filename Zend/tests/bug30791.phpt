--TEST--
Bug #30791 (magic methods (__sleep/__wakeup/__toString) call __call if object is overloaded)
--FILE--
<?php

function my_error_handler($errno, $errstr, $errfile, $errline)
{
    var_dump($errstr);
}
class a
{
    public $a = 4;
    function __call($a, $b)
    {
        return "unknown method";
    }
}
function fn1202984664()
{
    set_error_handler('my_error_handler');
    $b = new a();
    echo $b, "\n";
    $c = unserialize(serialize($b));
    echo $c, "\n";
    var_dump($c);
}
fn1202984664();
--EXPECT--
string(50) "Object of class a could not be converted to string"

string(50) "Object of class a could not be converted to string"

object(a)#2 (1) {
  ["a"]=>
  int(4)
}
