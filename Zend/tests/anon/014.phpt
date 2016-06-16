--TEST--
anonymous class trait binding
--FILE--
<?php

trait TaskTrait
{
    function run()
    {
        return 'Running...';
    }
}
function fn599232755()
{
    $class = new class
    {
        use TaskTrait;
    };
    var_dump($class->run());
}
fn599232755();
--EXPECTF--
string(10) "Running..."
