--TEST--
Bug #71441 (Typehinted Generator with return in try/finally crashes)
--FILE--
<?php

function fn155215304()
{
    $num = 2000;
    /* to be sure to be in wild memory */
    $add = str_repeat("1 +", $num);
    $gen = (eval(<<<PHP
return function (): \\Generator {
\ttry {
\t\t\$a = 1;
\t\t\$foo = \$a + {$add} \$a;
\t\treturn yield \$foo;
\t} finally {
\t\tprint "Ok
";
\t}
};
PHP
))();
    var_dump($gen->current());
    $gen->send("Success");
    var_dump($gen->getReturn());
}
fn155215304();
--EXPECT--
int(2002)
Ok
string(7) "Success"

