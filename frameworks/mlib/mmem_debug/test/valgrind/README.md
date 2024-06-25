在free接口的测试代码中涉及到重复free同一片内存

在free之前会读取一次内存, 这次读取其实是非法的.

但是在测试用例中, 这次读取可以忽略.

因此, 该文件用于抑制该警告.

## 如何添加抑制内容

是的，您可以使用 `Valgrind` 的 `--gen-suppressions` 选项来生成抑制信息，然后将其添加到抑制文件中，以便忽略特定的错误。例如，如果您想忽略某个 `Invalid read` 错误，可以按照以下步骤操作：

1. 运行 `Valgrind` 并使用 `--gen-suppressions=all` 选项：

   ```sh
   valgrind --leak-check=full --gen-suppressions=all --log-file=valgrind.log ./your_program
   ```

2. 在 `valgrind.log` 文件中找到与您想要忽略的错误相关的抑制信息。它看起来像这样：

   ```
   {
      insert_a_name_here
      Memcheck:Addr1
      fun:function_name
      ...
   }
   ```

3. 将抑制信息复制到一个新文件中，例如 `my_suppressions.supp`。

4. 在运行 `Valgrind` 时使用 `--suppressions` 选项指定抑制文件：

   ```sh
   valgrind --leak-check=full --suppressions=my_suppressions.supp ./your_program
   ```

这样，在运行 `Valgrind` 时，与抑制文件中列出的错误匹配的错误将被忽略。

请注意，忽略错误可能会掩盖潜在的问题。因此，在忽略错误之前，您应该仔细检查它是否是一个真正的问题。
