<?php

use PhpParser\{ParserFactory, PrettyPrinter};
use PhpParser\Node\{Expr, Stmt, Name};

error_reporting(E_ALL);

require __DIR__ . '/../PHP-Parser/lib/bootstrap.php';

$dirs = [
    __DIR__ . '/Zend/tests',
    __DIR__ . '/tests',
];
$it = new AppendIterator();
foreach ($dirs as $dir) {
    $it->append(new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    ));
}

$parser = (new ParserFactory)->create(ParserFactory::PREFER_PHP7);
$pp = new PrettyPrinter\Standard;

$blacklist = [
    'Zend/tests/030.phpt',
    'Zend/tests/nowdoc_015.phpt',
    'Zend/tests/bug35634.phpt',
    'Zend/tests/generators/errors/yield_outside_function_error.phpt',
    'Zend/tests/binary.phpt',
    'Zend/tests/bug70918.phpt',
    'tests/lang/bug35176.phpt',
    'tests/basic/027.phpt',
];

$forbidden = '/
    global|\$GLOBALS|debug_(print_)?backtrace|->getTrace|__LINE__|__halt_compiler
  | Reflection\w+::export|error_get_last
  | Parse\serror|on\sline\s\d|\#\d+\s\{main\}
  | (?:%s|\.php)\(\d+\)
/xi';

$count = 0;
foreach ($it as $file) {
    $name = $file->getPathName();
    if (!preg_match('/\.phpt$/', $name)) {
        continue;
    }

    foreach ($blacklist as $bl) {
        if (false !== strpos($name, $bl)) {
            continue 2;
        }
    }

    $code = file_get_contents($name);
    if (preg_match($forbidden, $code)) {
        continue;
    }

    $newCode = preg_replace_callback(
        '/(--FILE--\s+)(<\?php.*)(\s+--EXPECT)/s',
        function($matches) use($parser, $pp, $name, &$count) {
            $code = $matches[2];
            if (preg_match('/\?>(?!$)/', $code)) {
                return $matches[0];
            }

            try {
                $stmts = $parser->parse($code);

                $decls = [];
                $other = [];

                foreach ($stmts as $stmt) {
                    if ($stmt instanceof Stmt\Function_
                        || $stmt instanceof Stmt\ClassLike
                        || $stmt instanceof Stmt\Const_
                        || $stmt instanceof Stmt\Use_
                        || $stmt instanceof Stmt\GroupUse
                        || $stmt instanceof Stmt\Namespace_
                        || $stmt instanceof Stmt\Declare_
                    ) {
                        $decls[] = $stmt;
                        if (!empty($other)) {
                            if (!$stmt instanceof Stmt\Function_
                                && !($stmt instanceof Stmt\ClassLike && isEarlyBound($stmt))
                            ) {
                                return $matches[0];
                            }
                        }
                    } else {
                        $other[] = $stmt;
                    }
                }

                if (empty($other)) {
                    return $matches[0];
                }

                $fnName = 'fn' . mt_rand();
                $fn = new Stmt\Function_($fnName, [
                    'stmts' => $other, 
                ]);
                $call = new Expr\FuncCall(new Name($fnName));

                $stmts = array_merge($decls, [$fn, $call]);
                $code = $pp->prettyPrintFile($stmts);

                $count++;
                return $matches[1] . $code . $matches[3];
            } catch (PhpParser\Error $e) {
                return $matches[0];
            }
        },
        $code
    );

    if ($newCode !== $code) {
        file_put_contents($name, $newCode);
    }
}

echo "Ported $count tests\n";

function isEarlyBound(Stmt\ClassLike $node) {
    if (!$node instanceof Stmt\Class_) {
        return false;
    }
    if ($node->extends || $node->implements) {
        return false;
    }
    foreach ($node->stmts as $stmt) {
        if ($stmt instanceof Stmt\TraitUse) {
            return false;
        }
    }
    return true;
}
