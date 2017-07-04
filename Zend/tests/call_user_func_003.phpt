--TEST--
Testing call_user_func() with closures
--FILE--
<?php

function fn1896329829()
{
    $foo = function () {
        static $instance;
        if (is_null($instance)) {
            $instance = function () {
                return 'OK!';
            };
        }
        return $instance;
    };
    var_dump(call_user_func(array($foo, '__invoke'))->__invoke());
    var_dump(call_user_func(function () use(&$foo) {
        return $foo;
    }, '__invoke'));
}
fn1896329829();
--EXPECTF--
string(3) "OK!"
object(Closure)#%d (1) {
  ["static"]=>
  array(1) {
    ["instance"]=>
    object(Closure)#%d (0) {
    }
  }
}
