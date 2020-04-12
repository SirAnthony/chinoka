""" settings -- global constants """
from os import path

# Package base path (where the module script files are located)
PACKAGE_PATH = path.abspath(path.dirname(__file__))

# Path to the configuration file
CONFIG_PATH = path.join(PACKAGE_PATH, 'config.json')
MAP_PATH = path.join(PACKAGE_PATH, 'map.json')

# Log settings
LOG_ERROR_FORMAT = u"%(levelname)s at %(asctime)s in %(funcName)s in %(filename)s at line %(lineno)d: %(message)s"
LOG_ERROR_DATE = u'[%d.%m.%Y %I:%M:%S]'
LOG_PATH = path.join(PACKAGE_PATH, 'mqtt.log')
LOG_CONFIG = {'version': 1,
              'formatters': {'error': {'()': 'logging.Formatter',
                                       'format': LOG_ERROR_FORMAT,
                                       'datefmt': LOG_ERROR_DATE},
                             'debug': {'()': 'logging.Formatter',
                                       'format': u'%(asctime)s: %(message)s',
                                       'datefmt': u'[%d %b %I:%M:%S]'}},
              'handlers': {'console': {'class': 'logging.StreamHandler',
                                       'formatter': 'debug',
                                       'level': 'DEBUG'},
                          'file': {'class':'logging.handlers.RotatingFileHandler',
                                   'filename': LOG_PATH, 'maxBytes': 100000,
                                   'backupCount': 5, 'formatter':'error',
                                   'level': 'ERROR'}},
              'root': {'handlers': ('console', 'file'), 'level': 'DEBUG'}}
