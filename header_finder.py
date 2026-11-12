import pathlib
import re
import shutil

from rich.prompt import IntPrompt, InvalidResponse

root = pathlib.Path("/home/pdmurray/dev/fancontroller/")
src = root / "src/sketch_nov10a/"

includeds = ["Adafruit_seesaw.h", "seesaw_neopixel.h"]

class ChooserPrompt(IntPrompt):
    prompt_suffix = "> "

    def check_choice(self, value):
        return (
            self.choices is not None
            and isinstance(value, int)
            and value > 0
            and value <= len(self.choices)
        )

    def process_response(self, value):
        value = value.strip()
        try:
            return_value = self.response_type(value)
        except ValueError:
            raise InvalidResponse(self.validate_error_message)

        if self.choices is not None and not self.check_choice(return_value):
            raise InvalidResponse(self.illegal_choice_message)

        return return_value

    def make_prompt(self, default):
        prompt = self.prompt.copy()
        prompt.end = ""

        if self.show_choices and self.choices:
            choices = "\n"
            for i, choice in enumerate(self.choices, 1):
                choices += f"{i}. {choice}\n"
            prompt.append(choices, "prompt.choices")

        if (
            default != ...
            and self.show_default
            and isinstance(default, (str, self.response_type))
        ):
            prompt.append(" ")
            _default = self.render_default(default)
            prompt.append(_default)

        prompt.append(self.prompt_suffix)

        return prompt

def main():
    not_found = set()
    viewed = set()
    files = set(src.glob("**/*.h")) | set(src.glob("**/*.c")) | set(src.glob("**/*.ino"))
    while len(files) != 0:
        file = files.pop()
        viewed.add(file)

        if file in includeds:
            continue

        with open(file) as f:
            txt = f.read()

        # stdlibs = re.findall('#include <(?P<filename>.*)>', txt)
        libs = re.findall('#include "(?P<filename>.*)"', txt)

        for fname in libs:
            if fname in includeds:
                 continue

            choices = []
            for item in root.glob(f"**/{fname}"):
                if not item.is_relative_to(src):
                    choices.append(str(item.absolute()))

            if len(choices) > 1:
                # Try to reduce the number of choices. Some big lists have both
                # 'avr' and 'ATMEGA328P' in them; prefer ATMEGA328P
                reduced_choices = []
                for item in choices:
                    if "ATMEGA328P" in item:
                        reduced_choices.append(item)

                if not reduced_choices:
                    for item in choices:
                        if 'avr' in item:
                            reduced_choices.append(item)

                if not reduced_choices:
                    print("Could not filter choices...")
                else:
                    choices = reduced_choices

            if len(choices) == 1:
                target_file = choices[0]
            elif len(choices) > 1:
                choice = ChooserPrompt.ask(
                    "Choose a filename to copy", choices=choices
                ) - 1
                target_file = choices[choice]
            else:
                not_found.add(fname)
                continue

            newname = src / fname
            shutil.copy(target_file, newname)
            print(f"Copying {target_file} -> {newname}")
            files.add(newname)

    print(f"Could not find these: {not_found}")


if __name__ == '__main__':
    main()
