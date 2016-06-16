--TEST--
Bug #39721 (Runtime inheritance causes data corruption)
--FILE--
<?php

class test2
{
    private static $instances = 0;
    public $instance;
    public function __construct()
    {
        $this->instance = ++self::$instances;
    }
}
function fn670837505()
{
    $foo = new test2();
    if (is_object($foo)) {
        class test2_child extends test2
        {
        }
    }
    $child = new test2_child();
    echo $foo->instance . "\n";
    echo $child->instance . "\n";
}
fn670837505();
--EXPECT--
1
2
