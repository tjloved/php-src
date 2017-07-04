--TEST--
Bug #72598 (Reference is lost after array_slice())
--FILE--
<?php

function ref(&$ref)
{
    var_dump($ref);
}
function fn1593563184()
{
    new class
    {
        function __construct()
        {
            $args = [&$this];
            for ($i = 0; $i < 2; $i++) {
                $a = array_slice($args, 0, 1);
                call_user_func_array('ref', $a);
            }
        }
    };
}
fn1593563184();
--EXPECTF--
Warning: Parameter 1 to ref() expected to be a reference, value given in %sbug72598.php on line %d
object(class@anonymous)#1 (0) {
}

Warning: Parameter 1 to ref() expected to be a reference, value given in %sbug72598.php on line %d
object(class@anonymous)#1 (0) {
}
