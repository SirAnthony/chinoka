import os, getopt, sys
import json
import logging
import settings
try:
    from logging.config import dictConfig
    dictConfig(settings.LOG_CONFIG)
except ImportError:
    logging.basicConfig(level=logging.INFO,
        format=settings.LOG_ERROR_FORMAT)

VERSION = 0.1

def usage(prog):
    """
    Print command line help.
    Takes an application name to display.
    """
    # Won't strip indentation.
    logging.info("""
    Usage: %s [options]
    Options:
      -v, --version         show program's version number and exit
      -h, --help            show this help message and exit
      -c, --config <file>   use different config file
      -m, --map <file>      use custom mapping file
    """, prog)


def options(prog, version, argv):
    """
    Return options found in argv.
    prog will be to usage() when --help is requested.
    version will be printed for --version.
    The first items of argv must be an option, not the executable name like
    in sys.argv!
    The result has the format {section: {option: value}}
    """
    # For getopt we have to mention each option several times in the code,
    # unlike with the optparse module. But then that's more readable.
    # Also optparse doesn't keep information on which options where
    # actually given on the command line versus the hard-coded defaults.
    try:
        opts, args = getopt.getopt(argv, "vhc:", ["version", "help"])
    except getopt.GetoptError as err:
        logging.error(err)
        usage(prog)
        sys.exit(2)
    given = {}
    for o, a in opts:
        if o in ('-v', '--version'):
            logging.info('%s %s', prog, version)
            sys.exit()
        elif o in ('-h', '--help'):
            usage(prog)
            sys.exit()
        elif o in ('-c', '--config'):
            given.setdefault(None, {})["config"] = a
        elif o in ('-m', '--map'):
            given.setdefault(None, {})["map"] = a
        else:
            assert False, "getopt knew more than if"
    return given


class Config(dict):

    def __init__(self, indict=None):
        if indict is None:
            indict = {}
        dict.__init__(self, indict)

    def __getattr__(self, item):
        try:
            return self.__getitem__(item)
        except KeyError:
            c = Config()
            self.__setattr__(item, c)
            return c

    def __setattr__(self, item, value):
        if item in self.__dict__:
            dict.__setattr__(self, item, value)
        else:
            if isinstance(value, dict):
                self.__setitem__(item, Config(value))
            else:
                self.__setitem__(item, value)


run_opts = options(os.path.basename(sys.argv[0]), VERSION, sys.argv[1:])


class BaseConfig(Config):

    _instance = None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Config, cls).__new__(
                                    cls, *args, **kwargs)
        return cls._instance

    def __init__(self, indict=None):
        opts = run_opts.copy()
        if indict is not None:
            opts.update(indict)
        dict.__init__(self, opts)
        self.load()

    def load_paths(self, name, default):
        paths = [self.get(name), default]
        self[name] = None
        for path in paths:
            if not isinstance(path, str):
                continue
            try:
                stream = open(path, 'rU')
            except:
                pass
            else:
                self[name] = path
                return json.load(stream)
        return {}

    def load(self):
        config = self.load_paths('config', settings.CONFIG_PATH)
        for key, value in config.items():
            setattr(self, key, value)
        self.mapping = self.load_paths('map', settings.MAP_PATH)
        for k, v in self.mapping.items():
            if isinstance(v, str):
                self.mapping[k] = {'type': v}
