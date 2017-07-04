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
function fn559713776()
{
    $class = new class
    {
        use TaskTrait;
    };
    var_dump($class->run());
}
fn559713776();
--EXPECTF--
string(10) "Running..."
