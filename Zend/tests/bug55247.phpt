--TEST--
Request #55247 (Parser problem with static calls using string method name)
--FILE--
<?php

class Test
{
    public static function __callStatic($method, $arguments)
    {
        echo $method . PHP_EOL;
    }
    public function __call($method, $arguments)
    {
        echo $method . PHP_EOL;
    }
}
function fn1545407652()
{
    $method = 'method';
    $test = new Test();
    $test->method();
    $test->{$method}();
    $test->{'method'}();
    Test::method();
    Test::$method();
    Test::{'method'}();
}
fn1545407652();
--EXPECT--
method
method
method
method
method
method
